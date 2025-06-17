#ifndef VEC_2_H
#define VEC_2_H

#define VEC_2_ZERO (vec2){ .x = 0.f, .y = 0.f }
#define VEC_2_ONE  (vec2){ .x = 1.f, .y = 1.f }

typedef struct {
  float x;
  float y;
} vec2;

float vec2_angle(const vec2 *v, const vec2 *f);
float vec2_dot_product(const vec2 *v, const vec2 *f);
void vec2_multiply_scalar(vec2 *v, const float f);
float vec2_magnitude(const vec2 *v);
void vec2_normalize(vec2 *v);

void vec2_add(const vec2 *a, const vec2 *b, vec2 *res);
void vec2_subtract(const vec2 *a, const vec2 *b, vec2 *res);

#endif  // !VEC_2_H
