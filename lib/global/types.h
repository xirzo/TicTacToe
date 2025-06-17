#ifndef TYPES_H
#define TYPES_H

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

#endif  // !TYPES_H
