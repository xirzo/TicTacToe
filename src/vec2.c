#include "vec2.h"
#include <math.h>

float vec2_angle(const vec2 *v, const vec2 *f) {
  float dot_product = vec2_dot_product(v, f);

  float mag_v = vec2_magnitude(v);
  float mag_f = vec2_magnitude(f);

  if (mag_v < 0.001f || mag_f < 0.001f) {
    return 0.f;
  }

  return acos(dot_product / (mag_v * mag_f));
}

float vec2_dot_product(const vec2 *v, const vec2 *f) {
  return v->x * f->x + v->y * f->y;
}

void vec2_multiply_scalar(vec2 *v, const float f) {
  v->x *= f;
  v->y *= f;
}

float vec2_magnitude(const vec2 *v) {
  return sqrt(v->x * v->x + v->y * v->y);
}

void vec2_normalize(vec2 *v) {
  float mag = vec2_magnitude(v);

  if (mag > 0.001f) {
    vec2_multiply_scalar(v, 1.0f / mag);
  }
}

void vec2_add(const vec2 *a, const vec2 *b, vec2 *res) {
  res->x = a->x + b->x;
  res->y = a->y + b->y;
}

void vec2_subtract(const vec2 *a, const vec2 *b, vec2 *res) {
  res->x = a->x - b->x;
  res->y = a->y - b->y;
}
