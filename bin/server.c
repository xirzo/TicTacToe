#include "server.h"
#include "client.h"
#include "properties.h"

#include "game_state.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: remove global_state

static game_state_t g_state;

void *handle_client(void *arg) {
  thread_args_t *args = (thread_args_t *)arg;
  server_t *server = args->server;
  client_connection_t *con = &server->clients[args->client_index];

  printf("Client %d connected from %s\n", con->client_id, inet_ntoa(con->addr.sin_addr));

  switch (con->client_type) {
    case PLAYER_CIRCLE: {
      printf("Player role: CIRCLE\n");
      break;
    }
    case PLAYER_CROSS: {
      printf("Player role: CROSS\n");
      break;
    }
    case PLAYER_UNSET: {
      printf("Player role: UNSET\n");
      break;
    }
  }

  while (con->active) {
    client_message_t msg;
    ssize_t bytes_read = read(con->socket_fd, &msg, sizeof(client_message_t));

    if (bytes_read <= 0) {
      printf("Client %d disconnected\n", con->client_id);
      break;
    }

    if (bytes_read != sizeof(client_message_t)) {
      printf("Failed to read message, wrong message byte size\n");
      break;
    }

    switch (msg.type) {
      case CLIENT_MSG_SET_MARK: {
        // TODO: check win condition here
        printf(
            "Client %d set mark on: (%.2f, %.2f)\n",
            con->client_id,
            msg.data.position.x,
            msg.data.position.y
        );

        server_message_t pos_broadcast = { .type = SERVER_MSG_MARK_SET,
                                           .client_id = con->client_id,
                                           .timestamp = time(NULL),
                                           .data.position = msg.data.position };

        sr_send_message_to_all_except(server, con->client_id, &pos_broadcast);
        break;
      }

      default: {
        printf("Unknown message type %d from client %d\n", msg.type, con->client_id);
        break;
      }
    }
  }

  close(con->socket_fd);
  con->active = 0;

  pthread_mutex_lock(&server->clients_mutex);
  server->client_count--;
  pthread_mutex_unlock(&server->clients_mutex);

  free(arg);
  return NULL;
}

int is_game_finished() {
  return g_state.is_finished == 1;
}

int main(void) {
  state_init(&g_state);

  server_t *server = sr_create_server(SERVER_PORT, handle_client);

  if (!server) {
    fprintf(stderr, "error: Failed to create server\n");
    return 1;
  }

  sr_start_listen(server);

  printf("Starting game loop\n");

  while (!is_game_finished()) {
  }

  return 0;
}
