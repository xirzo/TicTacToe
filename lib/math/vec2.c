#include "vec2.h"
#include <math.h>

void vec2_multiply(vec2 *v, float f) {
  v->x *= f;
  v->y *= f;
}

float vec2_magnitude(vec2 *v) { return sqrt(v->x * v->x + v->y * v->y); }

void vec2_normalize(vec2 *v) {
  float mag = vec2_magnitude(v);

  if (mag > 0.001f) {
    vec2_multiply(v, 1.0f / mag);
  }
}
