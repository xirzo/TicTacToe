#ifndef BUTTON_H
#define BUTTON_H

#include <raylib.h>
#include "vec2.h"

typedef struct button button_t;

typedef struct button {
  /*

    NOT_PRESSED = -1,
    PRESSED = 0,
    HOVERED = 1,
  */
  int state;
  vec2 pos;
  vec2 size;
  Color color;
  void *param;
  void (*callback)(button_t *btn);
} button_t;

void button_init(button_t *btn, vec2 pos, vec2 size, Color color, void *param, void (*callback)());

/*
  should be put inside of the drawing loop
*/
void button_draw(button_t *btn);

/*
  checks the if the button is pressed
  should be put inside of the game loop
*/
void button_check(button_t *btn);

#endif  // ! BUTTON_H
