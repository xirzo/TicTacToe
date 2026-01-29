#include "server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

server_t *sr_create_server(unsigned short port, void *(*handle_client_callback)(void *)) {
  server_t *server = malloc(sizeof(server_t));

  if (!server) {
    fprintf(stderr, "SERVER ERROR: failed to allocate memory for server: %s\n", strerror(errno));
    return NULL;
  }

  server->addrlen = sizeof(server->servaddr);
  bzero(&server->servaddr, sizeof(server->servaddr));

  if ((server->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "SERVER ERROR: socket creation failed: %s\n", strerror(errno));
    free(server);
    return NULL;
  }

  server->servaddr.sin_family = AF_INET;
  server->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  server->servaddr.sin_port = htons(port);
  server->handle_client_callback = handle_client_callback;

  int optval = 1;
  if (setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0) {
    fprintf(stderr, "SERVER ERROR: setsockopt failed: %s\n", strerror(errno));
    close(server->fd);
    free(server);
    return NULL;
  }

  if (bind(server->fd, (const struct sockaddr *)&server->servaddr, sizeof(server->servaddr)) < 0) {
    fprintf(stderr, "SERVER ERROR: bind failed: %s\n", strerror(errno));
    close(server->fd);
    free(server);
    return NULL;
  }

  server->client_count = 0;
  if (pthread_mutex_init(&server->clients_mutex, NULL) != 0) {
    fprintf(stderr, "SERVER ERROR: mutex init failed: %s\n", strerror(errno));
    close(server->fd);
    free(server);
    return NULL;
  }

  for (size_t i = 0; i < MAX_PLAYERS; i++) {
    server->clients[i].active = 0;
  }

  fprintf(stderr, "SERVER: initialized successfully on port %d\n", port);
  return server;
}

void sr_destroy_server(server_t *server) {
  if (!server) {
    fprintf(stderr, "SERVER WARNING: attempt to destroy NULL server\n");
    return;
  }

  fprintf(stderr, "SERVER: shutting down...\n");

  pthread_mutex_lock(&server->clients_mutex);
  for (size_t i = 0; i < server->client_count; i++) {
    if (server->clients[i].active) {
      fprintf(stderr, "SERVER: disconnecting client %d\n", server->clients[i].client_id);
      server->clients[i].active = 0;
      close(server->clients[i].socket_fd);
      pthread_join(server->clients[i].thread_id, NULL);
    }
  }
  pthread_mutex_unlock(&server->clients_mutex);
  pthread_mutex_destroy(&server->clients_mutex);

  if (pthread_cancel(server->server_listen_thread) == 0) {
    pthread_join(server->server_listen_thread, NULL);
  }

  close(server->fd);
  free(server);
  fprintf(stderr, "SERVER: shutdown complete\n");
}

void *listen_thread_func(void *arg) {
  server_t *server = (server_t *)arg;

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(server->fd, (struct sockaddr *)&client_addr, &client_len);

    if (client_fd < 0) {
      fprintf(stderr, "SERVER ERROR: failed to accept connection: %s\n", strerror(errno));
      continue;
    }

    fprintf(stderr, "SERVER: new connection from %s\n", inet_ntoa(client_addr.sin_addr));

    int client_id = sr_add_client(server, client_fd, client_addr);

    if (client_id < 0) {
      fprintf(stderr, "SERVER ERROR: failed to add client (server full or error)\n");
      close(client_fd);
      continue;
    }

    fprintf(stderr, "SERVER: added client with ID %d\n", client_id);
  }
  return NULL;
}

void sr_start_listen(server_t *server) {
  if (listen(server->fd, MAX_PLAYERS) < 0) {
    fprintf(stderr, "SERVER ERROR: failed listening: %s\n", strerror(errno));
    return;
  }

  fprintf(stderr, "SERVER: listening on port %d...\n", ntohs(server->servaddr.sin_port));
  fprintf(stderr, "SERVER: waiting for connections...\n");

  if (pthread_create(&server->server_listen_thread, NULL, listen_thread_func, server) != 0) {
    fprintf(stderr, "SERVER ERROR: failed to create listen thread: %s\n", strerror(errno));
  }
}

int sr_add_client(server_t *server, int socket_fd, struct sockaddr_in addr) {
  if (server->client_count >= MAX_PLAYERS) {
    fprintf(stderr, "SERVER: maximum clients reached (%d)\n", MAX_PLAYERS);
    return -1;
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
    if (!args) {
      fprintf(stderr, "SERVER ERROR: failed to allocate client args\n");
      con->active = 0;
      pthread_mutex_unlock(&server->clients_mutex);
      return -1;
    }
    args->client_index = i;
    args->server = server;

    if (pthread_create(
            &server->clients[i].thread_id, NULL, server->handle_client_callback, (void *)args
        )
        != 0) {
      fprintf(stderr, "SERVER ERROR: failed to create client thread: %s\n", strerror(errno));
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
      if (send(server->clients[i].socket_fd, message, sizeof(server_message_t), 0) < 0) {
        fprintf(
            stderr,
            "SERVER ERROR: failed to send to client %d: %s\n",
            server->clients[i].client_id,
            strerror(errno)
        );
      }
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
      if (send(server->clients[i].socket_fd, message, sizeof(server_message_t), 0) < 0) {
        fprintf(
            stderr,
            "SERVER ERROR: failed to send to client %d: %s\n",
            server->clients[i].client_id,
            strerror(errno)
        );
      }
    }
  }

  pthread_mutex_unlock(&server->clients_mutex);
}

void sr_send_message_to_client(server_t *server, int client_id, const server_message_t *message) {
  pthread_mutex_lock(&server->clients_mutex);

  for (int i = 0; i < MAX_PLAYERS; i++) {
    if (server->clients[i].active && server->clients[i].client_id == client_id) {
      if (send(server->clients[i].socket_fd, message, sizeof(server_message_t), 0) < 0) {
        fprintf(
            stderr,
            "SERVER ERROR: failed to send to client %d: %s\n",
            server->clients[i].client_id,
            strerror(errno)
        );
      }
      break;
    }
  }

  pthread_mutex_unlock(&server->clients_mutex);
}
