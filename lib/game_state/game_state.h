#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "types.h"
#include "client.h"

#define BOARD_SIDE 3

typedef struct game_state {
  board_cell cells[BOARD_SIDE][BOARD_SIDE];
  client_t client;
  int is_finished;
  int waiting_for_response;
  int pending_x;
  int pending_y;
} game_state_t;

typedef struct server_game_state {
  board_cell cells[BOARD_SIDE][BOARD_SIDE];
  int is_finished;
  player_role turn_role;
} server_game_state_t;

void state_init(game_state_t *state);
void server_state_init(server_game_state_t *state);

#endif  // !GAME_STATE_H
