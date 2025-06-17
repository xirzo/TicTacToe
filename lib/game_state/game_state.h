#ifndef GAME_STATE_H
#define GAME_STATE_H

#define BOARD_SIDE 3

typedef enum {
  PLAYER_CIRCLE = 0,
  PLAYER_CROSS,
  PLAYER_UNSET,
} player_role;

typedef enum {
  CELL_EMPTY = -1,
  CELL_CIRCLE = 0,
  CELL_CROSS = 1,
} board_cell;

typedef struct game_state {
  board_cell cells[BOARD_SIDE][BOARD_SIDE];
  player_role role;
} game_state_t;

void state_init(game_state_t *state);

#endif  // !GAME_STATE_H
