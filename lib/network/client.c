#include "client.h"
#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

int sr_client_connect(client_t *client, const char *server_ip, unsigned short port) {
  if (!server_ip) {
    printf("ip is NULL\n");
    return -1;
  }

  client->addr_len = sizeof(client->server_addr);

  client->server_addr.sin_family = AF_INET;
  client->server_addr.sin_port = htons(port);
  client->server_addr.sin_addr.s_addr = inet_addr(server_ip);

  if (inet_pton(AF_INET, server_ip, &client->server_addr.sin_addr) <= 0) {
    printf("invalid address or address not supported\n");
    return -1;
  }

  if ((client->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("socket creation failed...\n");
    return -1;
  }

  if (connect(
          client->server_fd, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr)
      )
      < 0) {
    printf("connection with the server failed...\n");
    return -1;
  }

  printf("connected to the server..\n");
  return 0;
}

void sr_client_close(client_t *client) {
  close(client->server_fd);
}

int sr_receive_server_message(client_t *client, server_message_t *msg) {
  if (!client || !msg) {
    return -1;
  }

  static int made_nonblocking = 0;

  if (!made_nonblocking) {
    int flags = fcntl(client->server_fd, F_GETFL, 0);
    fcntl(client->server_fd, F_SETFL, flags | O_NONBLOCK);
    made_nonblocking = 1;
  }

  ssize_t bytes_read = recv(client->server_fd, msg, sizeof(server_message_t), 0);

  if (bytes_read == sizeof(server_message_t)) {
    return 0;
  } else if (bytes_read == 0) {
    return -1;
  } else if (bytes_read < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -3;
    }
    return -2;
  } else {
    return -2;
  }
}

int sr_send_message_to_server(client_t *client, const client_message_t *msg) {
  if (!client || !msg) {
    return -1;
  }

  ssize_t bytes_sent = send(client->server_fd, msg, sizeof(client_message_t), 0);

  if (bytes_sent != sizeof(client_message_t)) {
    perror("ERROR in sr_send_message_to_server");
    return -2;
  }

  return 0;
}
