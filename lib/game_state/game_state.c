#include "game_state.h"
#include "types.h"

void state_init(game_state_t *state) {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      state->cells[i][j] = CELL_EMPTY;
    }
  }

  state->is_finished = 0;
  state->waiting_for_response = 0;
  state->pending_x = -1;
  state->pending_y = -1;
}

void server_state_init(server_game_state_t *state) {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      state->cells[i][j] = CELL_EMPTY;
    }
  }

  state->is_finished = 0;
  state->turn_role = PLAYER_CIRCLE;
}
