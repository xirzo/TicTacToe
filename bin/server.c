#include "server.h"
#include "client.h"
#include "properties.h"

#include "game_state.h"
#include "types.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// TODO: remove global_state

static server_game_state_t g_state;

static void *handle_client(void *arg);
static int check_win();
static int check_draw();

int main(void) {
  server_state_init(&g_state);

  server_t *server = sr_create_server(SERVER_PORT, handle_client);

  if (!server) {
    fprintf(stderr, "error: Failed to create server\n");
    return 1;
  }

  sr_start_listen(server);

  printf("Starting game loop\n");

  while (1) {
  }

  return 0;
}

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
      case CLIENT_MSG_RESTART: {
        printf("Client %d initialized game restart\n", con->client_id);
        server_state_init(&g_state);

        server_message_t restart_msg = {
          .type = SERVER_MSG_RESTART,
          .client_id = con->client_id,
          .timestamp = time(NULL),
        };

        sr_send_message_to_all(server, &restart_msg);

        break;
      }
      case CLIENT_MSG_SET_MARK: {
        if (g_state.turn_role != con->client_type) {
          printf(
              "Client %d tried to send mark on: (%.2f, %.2f)\n",
              con->client_id,
              msg.data.position.x,
              msg.data.position.y
          );
          break;
        }

        switch (con->client_type) {
          case PLAYER_CIRCLE: {
            printf("Changed role to: CROSS\n");
            g_state.turn_role = PLAYER_CROSS;
            break;
          }
          case PLAYER_CROSS: {
            printf("Changed role to: CIRCLE\n");

            g_state.turn_role = PLAYER_CIRCLE;
            break;
          }
        }

        printf(
            "Client %d sent mark on: (%.2f, %.2f)\n",
            con->client_id,
            msg.data.position.x,
            msg.data.position.y
        );

        int x = (int)msg.data.position.x;
        int y = (int)msg.data.position.y;

        switch (con->client_type) {
          case PLAYER_CIRCLE: {
            g_state.cells[x][y] = CELL_PLAYER;
            break;
          }
          case PLAYER_CROSS: {
            g_state.cells[x][y] = CELL_ENEMY;
            break;
          }
        }

        server_message_t allow_msg = {
          .type = SERVER_MSG_ALLOW,
          .client_id = con->client_id,
          .timestamp = time(NULL),
        };

        sr_send_message_to_client(server, con->client_id, &allow_msg);

        server_message_t pos_broadcast = { .type = SERVER_MSG_MARK_SET,
                                           .client_id = con->client_id,
                                           .timestamp = time(NULL),
                                           .data.position = msg.data.position };

        sr_send_message_to_all_except(server, con->client_id, &pos_broadcast);

        if (check_win() || check_draw()) {
          server_message_t game_end_message = { .type = SERVER_MSG_GAME_END,
                                                .client_id = con->client_id,
                                                .timestamp = time(NULL),
                                                .data.position = msg.data.position };
          sr_send_message_to_all(server, &game_end_message);
          g_state.is_finished = 1;
        }

        break;
      }

      default: {
        printf("Unknown message type %d from client %d\n", msg.type, con->client_id);
        break;
      }

        printf("a\n");
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

int check_column(int i) {
  if (g_state.cells[i][0] == CELL_EMPTY) {
    return 0;
  }

  for (int j = 1; j < BOARD_SIDE; j++) {
    if (g_state.cells[i][j] != g_state.cells[i][0]) {
      return 0;
    }
  }

  return 1;
}

int check_row(int j) {
  if (g_state.cells[0][j] == CELL_EMPTY) {
    return 0;
  }

  for (int i = 1; i < BOARD_SIDE; i++) {
    if (g_state.cells[i][j] != g_state.cells[0][j]) {
      return 0;
    }
  }

  return 1;
}

int check_diag() {
  if (g_state.cells[0][0] != CELL_EMPTY) {
    int win = 1;

    for (int i = 1; i < BOARD_SIDE; i++) {
      if (g_state.cells[i][i] != g_state.cells[0][0]) {
        win = 0;
        break;
      }
    }

    if (win) {
      return 1;
    }
  }

  if (g_state.cells[0][BOARD_SIDE - 1] != CELL_EMPTY) {
    int win = 1;

    for (int i = 1; i < BOARD_SIDE; i++) {
      if (g_state.cells[i][BOARD_SIDE - 1 - i] != g_state.cells[0][BOARD_SIDE - 1]) {
        win = 0;
        break;
      }
    }

    if (win) {
      return 1;
    }
  }

  return 0;
}

int check_win() {
  for (int i = 0; i < BOARD_SIDE; i++) {
    if (check_row(i) || check_column(i)) {
      return 1;
    }
  }

  if (check_diag()) {
    return 1;
  }

  return 0;
}

int check_draw() {
  for (int i = 0; i < BOARD_SIDE; i++) {
    for (int j = 0; j < BOARD_SIDE; j++) {
      if (g_state.cells[i][j] == CELL_EMPTY) {
        return 0;
      }
    }
  }

  return !check_win();
}
