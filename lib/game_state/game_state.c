#include "game_state.h"

void state_init(game_state_t *state) {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      state->cells[i][j] = CELL_EMPTY;
    }
  }
}
