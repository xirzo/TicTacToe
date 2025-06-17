#include "server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024

server_t *sr_create_server(unsigned short port, void *(*handle_client_callback)(void *)) {
  server_t *server = malloc(sizeof(server_t));

  if (!server) {
    perror("failed to allocate memory for server");
    return NULL;
  }

  server->addrlen = sizeof(server->servaddr);
  bzero(&server->servaddr, sizeof(server->servaddr));

  if ((server->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failed");
    free(server);
    return NULL;
  }

  server->servaddr.sin_family = AF_INET;
  server->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  server->servaddr.sin_port = htons(port);
  server->handle_client_callback = handle_client_callback;

  int optval = 1;
  setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

  if (bind(server->fd, (const struct sockaddr *)&server->servaddr, sizeof(server->servaddr)) < 0) {
    perror("bind failed");
    close(server->fd);
    free(server);
    return NULL;
  }

  server->client_count = 0;
  pthread_mutex_init(&server->clients_mutex, NULL);

  for (size_t i = 0; i < MAX_PLAYERS; i++) {
    server->clients[i].active = 0;
  }

  return server;
}

void sr_destroy_server(server_t *server) {
  if (!server) {
    return;
  }

  pthread_mutex_lock(&server->clients_mutex);
  for (size_t i = 0; i < server->client_count; i++) {
    if (server->clients[i].active) {
      server->clients[i].active = 0;
      close(server->clients[i].socket_fd);
      pthread_join(server->clients[i].thread_id, NULL);
    }
  }
  pthread_mutex_unlock(&server->clients_mutex);
  pthread_mutex_destroy(&server->clients_mutex);
  close(server->fd);
  free(server);
}

void sr_start_listen(server_t *server) {
  if (listen(server->fd, MAX_PLAYERS) < 0) {
    perror("failed listening");
    return;
  }

  printf("Server listening on port %d...\n", ntohs(server->servaddr.sin_port));
  printf("Waiting for connections...\n");

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server->fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0) {
      perror("failed to accept connection");
      continue;
    }

    printf("New connection accepted from %s\n", inet_ntoa(client_addr.sin_addr));

    int client_id = sr_add_client(server, client_fd, client_addr);

    if (client_id < 0) {
      printf("Failed to add client (server full or error)\n");
      close(client_fd);
      continue;
    }

    printf("Successfully added client with ID %d\n", client_id);
  }
}

int sr_add_client(server_t *server, int socket_fd, struct sockaddr_in addr) {
  if (server->client_count >= MAX_PLAYERS) {
    return 1;
  }

  pthread_mutex_lock(&server->clients_mutex);

  for (int i = 0; i < MAX_PLAYERS; i++) {
    client_connection_t *con = &server->clients[i];

    if (con->active) {
      continue;
    }

    con->socket_fd = socket_fd;
    con->addr = addr;
    con->active = 1;
    con->client_id = i;
    con->client_type = i % 2;

    thread_args_t *args = malloc(sizeof(thread_args_t));
    args->client_index = i;
    args->server = server;

    if (pthread_create(
            &server->clients[i].thread_id, NULL, server->handle_client_callback, (void *)args
        )
        < 0) {
      con->active = 0;
      free(args);
      pthread_mutex_unlock(&server->clients_mutex);
      return -1;
    }

    server->client_count++;
    pthread_mutex_unlock(&server->clients_mutex);
    return con->client_id;
  }

  pthread_mutex_unlock(&server->clients_mutex);
  return -1;
}

void sr_send_message_to_all(server_t *server, const server_message_t *message) {
  pthread_mutex_lock(&server->clients_mutex);

  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (server->clients[i].active) {
      send(server->clients[i].socket_fd, message, sizeof(server_message_t), 0);
    }
  }

  pthread_mutex_unlock(&server->clients_mutex);
}

void sr_send_message_to_all_except(
    server_t *server,
    int except_client_id,
    const server_message_t *message
) {
  pthread_mutex_lock(&server->clients_mutex);

  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (server->clients[i].active && server->clients[i].client_id != except_client_id) {
      send(server->clients[i].socket_fd, message, sizeof(server_message_t), 0);
    }
  }

  pthread_mutex_unlock(&server->clients_mutex);
}

void sr_send_message_to_client(server_t *server, int client_id, const server_message_t *message) {
  pthread_mutex_lock(&server->clients_mutex);

  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (server->clients[i].active && server->clients[i].client_id == client_id) {
      send(server->clients[i].socket_fd, message, sizeof(server_message_t), 0);
      break;
    }
  }

  pthread_mutex_unlock(&server->clients_mutex);
}

// TODO: add logger
