#include "client.h"
#include "properties.h"
#include "raylib.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

#include "game_state.h"
#include "button.h"
#include "vec2.h"

void button_on_pressed(button_t *btn) {
  game_state_t *state = (game_state_t *)btn->param;

  vec2 offset;
  vec2_subtract(&SCREEN_CENTER, &btn->pos, &offset);

  // TODO: 1. make offset automatic (replace constants)
  vec2_multiply_scalar(&offset, 1.f / 100);

  offset.x += 1.f;
  offset.y += 1.f;

  state->cells[(int)offset.x][(int)offset.y] = CELL_CROSS;

  btn->color = GREEN;
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

      button_init(btn, pos, size, RED, (void *)state, button_on_pressed);
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

    BeginDrawing();

    ClearBackground(BLACK);

    for (int i = 0; i < BOARD_SIDE * BOARD_SIDE; i++) {
      button_draw(&btns[i]);
    }

    EndDrawing();
  }

  free(btns);
  CloseWindow();
  return 0;
}
