#include "server.h"
#include "properties.h"

#include "game_state.h"
#include <stdio.h>

int main(void) {
  game_state_t state;

  state_init(&state);

  server_t *server = sr_create_server(g_Properties.SERVER_PORT);

  if (!server) {
    fprintf(stderr, "error: Failed to create server\n");
    return 1;
  }

  sr_start_listen(server);
  return 0;
}
