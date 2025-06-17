#ifndef PROPERTIES_H
#define PROPERTIES_H

typedef struct properties {
  const int SCREEN_WIDTH;
  const int SCREEN_HEIGHT;
  const unsigned short SERVER_PORT;
} properties_t;

static inline const properties_t *get_properties(void) {
  static const properties_t g_Properties = {
      .SCREEN_WIDTH = 900,
      .SCREEN_HEIGHT = 600,
      .SERVER_PORT = 8080,
  };
  return &g_Properties;
}

#define g_Properties (*get_properties())

#endif // !PROPERTIES_H
