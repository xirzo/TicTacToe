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

  vec2 offset;
  vec2_subtract(&SCREEN_CENTER, &btn->pos, &offset);

  // TODO: 1. make offset automatic (replace constants)
  vec2_multiply_scalar(&offset, 1.f / 100);

  offset.x += 1.f;
  offset.y += 1.f;

  int x = (int)offset.x;
  int y = (int)offset.y;

  state->cells[x][y] = CELL_PLAYER;
  btn->color = PLAYER_COLOR;

  client_message_t msg = {
    .type = CLIENT_MSG_SET_MARK,
    .data.position = (vec2){ x, y },
  };

  sr_send_message_to_server(&state->client, &msg);
}

// TODO: maybe put a while loop so that you can`t move until new pos it set

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
    default: {
      fprintf(stderr, "error: Wrong message server type\n");
      break;
    }
  }
}

void init_buttons(button_t *btns, game_state_t *state) {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      button_t *btn = &btns[i + BOARD_SIDE * j];

      // TODO: 2. make offset automatic (replace constants)
      vec2 pos;
      vec2 size = (vec2){ 80, 80 };
      vec2 offset = (vec2){ (i - 1) * 100, (j - 1) * 100 };
      vec2_subtract(&SCREEN_CENTER, &offset, &pos);

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
    for (int i = 0; i < BOARD_SIDE * BOARD_SIDE; i++) {
      button_check(&btns[i]);
    }

    receive_server_message(&state);

    BeginDrawing();

    ClearBackground(BLACK);

    update_buttons(btns, &state);

    EndDrawing();
  }

  free(btns);
  CloseWindow();
  return 0;
}
