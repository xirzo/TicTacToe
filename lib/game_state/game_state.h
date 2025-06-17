#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"
#include "client.h"

#define BOARD_SIDE 3

typedef struct game_state {
  board_cell cells[BOARD_SIDE][BOARD_SIDE];
  client_t client;
  int is_finished;
} game_state_t;

void state_init(game_state_t *state);

#endif  // !GAME_STATE_H
