#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include "vec2.h"
#include <netinet/in.h>
#include <sys/socket.h>

typedef enum {
  CLIENT_MSG_SET_MARK = 1,
} client_message_type;

typedef struct client_message {
  client_message_type type;
  int client_id;

  union {
    vec2 position;
    char text[256];
  } data;
} client_message_t;

typedef struct client {
  int server_fd;
  struct sockaddr_in server_addr;
  socklen_t addr_len;
} client_t;

int sr_send_message_to_server(client_t *client, const client_message_t *msg);
void sr_client_close(client_t *client);
int sr_receive_server_message(client_t *client, server_message_t *msg);
int sr_client_connect(client_t *client, const char *server_ip, unsigned short port);

#endif  // !CLIENT_H
