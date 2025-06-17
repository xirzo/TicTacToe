#include "button.h"
#include <raylib.h>

void button_init(
    button_t *btn,
    vec2 pos,
    vec2 size,
    Color color,
    Texture2D *texture,
    void *param,
    void (*callback)()
) {
  btn->state = -1;
  btn->pos = pos;
  btn->size = size;
  btn->color = color;
  btn->texture = texture;
  btn->param = param;
  btn->callback = callback;
}

void button_draw(button_t *btn) {
  if (btn->texture) {
    DrawTexture(*btn->texture, btn->pos.x, btn->pos.y, btn->color);
    return;
  }

  DrawRectangle(btn->pos.x, btn->pos.y, btn->size.x, btn->size.y, btn->color);
}

void button_check(button_t *btn) {
  Vector2 mos_pos = GetMousePosition();

  if (CheckCollisionPointRec(
          mos_pos, (Rectangle){ btn->pos.x, btn->pos.y, btn->size.x, btn->size.y }
      )) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      btn->state = 0;

      return;
    } else {
      btn->state = 1;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
      btn->callback(btn);
    }
  }

  btn->state = -1;
}
