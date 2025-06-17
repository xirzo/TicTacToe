#ifndef TYPES_H
#define TYPES_H

typedef enum {
  PLAYER_CIRCLE = 0,
  PLAYER_CROSS = 1,
} player_role;

typedef enum {
  CELL_EMPTY = -1,
  CELL_PLAYER = 0,
  CELL_ENEMY = 1,
} board_cell;

#endif  // !TYPES_H
