#include "client.h"
#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int sr_client_connect(client_t *client, const char *server_ip, unsigned short port) {
  if (!server_ip) {
    fprintf(stderr, "CLIENT ERROR: ip is NULL\n");
    return -1;
  }

  client->addr_len = sizeof(client->server_addr);

  client->server_addr.sin_family = AF_INET;
  client->server_addr.sin_port = htons(port);
  client->server_addr.sin_addr.s_addr = inet_addr(server_ip);

  if (inet_pton(AF_INET, server_ip, &client->server_addr.sin_addr) <= 0) {
    fprintf(stderr, "CLIENT ERROR: invalid address or address not supported\n");
    return -1;
  }

  if ((client->server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "CLIENT ERROR: socket creation failed: %s\n", strerror(errno));
    return -1;
  }

  if (connect(
          client->server_fd, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr)
      )
      < 0) {
    fprintf(stderr, "CLIENT ERROR: connection with the server failed: %s\n", strerror(errno));
    return -1;
  }

  fprintf(stderr, "CLIENT: connected to the server\n");
  return 0;
}

void sr_client_close(client_t *client) {
  close(client->server_fd);
  fprintf(stderr, "CLIENT: connection closed\n");
}

int sr_receive_server_message(client_t *client, server_message_t *msg) {
  if (!client || !msg) {
    fprintf(stderr, "CLIENT ERROR: invalid arguments\n");
    return -1;
  }

  static int made_nonblocking = 0;

  if (!made_nonblocking) {
    int flags = fcntl(client->server_fd, F_GETFL, 0);
    if (fcntl(client->server_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
      fprintf(stderr, "CLIENT ERROR: failed to set non-blocking mode: %s\n", strerror(errno));
      return -1;
    }
    made_nonblocking = 1;
  }

  ssize_t bytes_read = recv(client->server_fd, msg, sizeof(server_message_t), 0);

  if (bytes_read == sizeof(server_message_t)) {
    return 0;
  } else if (bytes_read == 0) {
    fprintf(stderr, "CLIENT: server disconnected\n");
    return -1;
  } else if (bytes_read < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return -3;
    }
    fprintf(stderr, "CLIENT ERROR: receive failed: %s\n", strerror(errno));
    return -2;
  } else {
    fprintf(stderr, "CLIENT ERROR: incomplete message received\n");
    return -2;
  }
}

int sr_send_message_to_server(client_t *client, const client_message_t *msg) {
  if (!client || !msg) {
    fprintf(stderr, "CLIENT ERROR: invalid arguments\n");
    return -1;
  }

  ssize_t bytes_sent = send(client->server_fd, msg, sizeof(client_message_t), 0);

  if (bytes_sent != sizeof(client_message_t)) {
    fprintf(stderr, "CLIENT ERROR: failed to send message: %s\n", strerror(errno));
    return -2;
  }

  return 0;
}
