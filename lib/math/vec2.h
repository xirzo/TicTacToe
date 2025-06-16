#ifndef VEC_2_H
#define VEC_2_H

typedef struct {
  float x;
  float y;
} vec2;

void vec2_multiply(vec2 *v, float f);
float vec2_magnitude(vec2 *v);
void vec2_normalize(vec2 *v);

#endif // !VEC_2_H
