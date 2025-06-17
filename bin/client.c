#include "client.h"
#include "properties.h"
#include "raylib.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_state.h"
#include "button.h"
#include "server.h"
#include "types.h"
#include "vec2.h"

void button_on_pressed(button_t *btn) {
  game_state_t *state = (game_state_t *)btn->param;

  float width = BOARD_SIDE * BOARD_BUTTON_SIZE + (BOARD_SIDE - 1) * BOARD_SPACING;

  float start_y = (float)SCREEN_WIDTH / 2 - width / 2;
  float start_x = (float)SCREEN_HEIGHT / 2 - width / 2;

  float relative_x = btn->pos.x - start_y;
  float relative_y = btn->pos.y - start_x;

  int x = (int)(relative_x / (BOARD_BUTTON_SIZE + BOARD_SPACING));
  int y = (int)(relative_y / (BOARD_BUTTON_SIZE + BOARD_SPACING));

  if (x >= 0 && x < BOARD_SIDE && y >= 0 && y < BOARD_SIDE && state->cells[x][y] == CELL_EMPTY) {
    state->cells[x][y] = CELL_PLAYER;
    btn->color = PLAYER_COLOR;

    client_message_t msg = {
      .type = CLIENT_MSG_SET_MARK,
      .data.position = (vec2){ x, y },
    };

    sr_send_message_to_server(&state->client, &msg);
  }
}

void receive_server_message(game_state_t *state) {
  client_t *client = &state->client;
  server_message_t msg;

  if ((sr_receive_server_message(client, &msg)) < 0) {
    return;
  }

  switch (msg.type) {
    case SERVER_MSG_MARK_SET: {
      fprintf(stderr, "Enemy mark set\n");
      int x = (int)msg.data.position.x;
      int y = (int)msg.data.position.y;
      state->cells[x][y] = CELL_ENEMY;
      break;
    }
    case SERVER_MSG_MARK_GAME_END: {
      fprintf(stderr, "Game finished\n");
      state->is_finished = 1;
      break;
    }
    default: {
      fprintf(stderr, "error: Wrong message server type\n");
      break;
    }
  }
}

void init_buttons(button_t *btns, game_state_t *state) {
  float width = BOARD_SIDE * BOARD_BUTTON_SIZE + (BOARD_SIDE - 1) * BOARD_SPACING;

  float start_x = (float)SCREEN_WIDTH / 2 - width / 2;
  float start_y = (float)SCREEN_HEIGHT / 2 - width / 2;

  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      button_t *btn = &btns[i + BOARD_SIDE * j];
      vec2 pos = { start_x + i * (BOARD_BUTTON_SIZE + BOARD_SPACING),
                   start_y + j * (BOARD_BUTTON_SIZE + BOARD_SPACING) };
      vec2 size = { BOARD_BUTTON_SIZE, BOARD_BUTTON_SIZE };

      button_init(btn, pos, size, INITIAL_COLOR, (void *)state, button_on_pressed);
    }
  }
}

void update_buttons(button_t *btns, game_state_t *state) {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      button_t *btn = &btns[i + BOARD_SIDE * j];

      switch (state->cells[i][j]) {
        case CELL_EMPTY: {
          btn->color = INITIAL_COLOR;
          break;
        }
        case CELL_PLAYER: {
          btn->color = PLAYER_COLOR;
          break;
        }
        case CELL_ENEMY: {
          btn->color = ENEMY_COLOR;
          break;
        }
      }
      button_draw(btn);
    }
  }
}

int main(void) {
  game_state_t state;

  state_init(&state);

  sr_client_connect(&state.client, SERVER_IP, SERVER_PORT);

  SetTraceLogLevel(LOG_ERROR);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TicTacToe");
  SetTargetFPS(FPS);

  button_t *btns = malloc(BOARD_SIDE * BOARD_SIDE * sizeof(button_t));

  init_buttons(btns, &state);

  while (!WindowShouldClose()) {
    if (!state.is_finished) {
      for (int i = 0; i < BOARD_SIDE * BOARD_SIDE; i++) {
        button_check(&btns[i]);
      }
    }

    receive_server_message(&state);

    BeginDrawing();

    ClearBackground(BLACK);

    if (!state.is_finished) {
      update_buttons(btns, &state);
    }

    if (state.is_finished) {
      const char *text = "Game Finished!";
      int text_width = MeasureText(text, FONT_SIZE);
      DrawText(text, SCREEN_WIDTH / 2 - text_width / 2, SCREEN_HEIGHT / 2, FONT_SIZE, WHITE);
    }

    EndDrawing();
  }

  free(btns);
  CloseWindow();
  return 0;
}

// TODO: restart
