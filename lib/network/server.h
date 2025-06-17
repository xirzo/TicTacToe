#ifndef SERVER_H
#define SERVER_H

#include "vec2.h"
#include <netinet/in.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>

#define MAX_PLAYERS 2

typedef enum {
  CLIENT_CIRCLE = 0,
  CLIENT_CROSS = 1,
} client_type;

typedef enum {
  SERVER_MSG_MARK_SET = 1,
} server_message_type;

typedef struct server_message {
  server_message_type type;
  int client_id;
  unsigned int timestamp;
  union {
    vec2 position;
  } data;
} server_message_t;

typedef struct client_connection {
  int socket_fd;
  struct sockaddr_in addr;
  int active;
  int is_main;
  pthread_t thread_id;
  client_type client_type;
  int client_id;
} client_connection_t;

typedef struct server {
  int fd;
  struct sockaddr_in servaddr;
  socklen_t addrlen;

  client_connection_t clients[MAX_PLAYERS];
  size_t client_count;
  pthread_mutex_t clients_mutex;
} server_t;

typedef struct thread_args {
  server_t *server;
  int client_index;
} thread_args_t;

server_t *sr_create_server(unsigned short port);
void sr_destroy_server(server_t *server);
void sr_start_listen(server_t *server);
int sr_add_client(server_t *server, int socket_fd, struct sockaddr_in addr);
void sr_send_message_to_all(server_t *server, const server_message_t *message);
void sr_send_message_to_all_except(server_t *server, int except_client_id,
                                   const server_message_t *message);
void sr_send_message_to_client(server_t *server, int client_id,
                               const server_message_t *message);

#endif // !SERVER_H
