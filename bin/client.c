#include "properties.h"
#include "raylib.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_state.h"
#include "button.h"
#include "vec2.h"

void button_on_pressed(button_t *btn) {
  printf("Button pressed: x: %f y: %f\n", btn->pos.x, btn->pos.y);
}

void init_buttons(button_t *btns, const vec2 *screen_center) {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      button_t *btn = &btns[i + BOARD_SIDE * j];

      vec2 pos;
      vec2 offset = (vec2){ (i - 1) * 100, (j - 1) * 100 };
      vec2_subtract(screen_center, &offset, &pos);
      vec2 size = (vec2){ 80, 80 };
      button_init(btn, pos, size, button_on_pressed);
    }
  }
}

int main(void) {
  const vec2 SCREEN_CENTER = { (float)g_Properties.SCREEN_WIDTH / 2,
                               (float)g_Properties.SCREEN_HEIGHT / 2 };

  game_state_t state;

  state_init(&state);

  InitWindow(g_Properties.SCREEN_WIDTH, g_Properties.SCREEN_HEIGHT, "TicTacToe");

  SetTargetFPS(60);

  button_t *btns = malloc(BOARD_SIDE * BOARD_SIDE * sizeof(button_t));

  init_buttons(btns, &SCREEN_CENTER);

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
