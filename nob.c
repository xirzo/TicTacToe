#define NOB_IMPLEMENTATION
#include "nob.h"

#include <stdlib.h>
#include <string.h>

#define CLIENT_EXE_NAME "tictactoe_client"
#define SERVER_EXE_NAME "tictactoe_server"

#define ASSETS_DIR "assets/"
#define BUILD_DIR "build/"
#define SRC_DIR "src/"

bool is_program_installed(const char *name);
bool is_library_installed(const char *name);

int main(int argc, char **argv)
{
  NOB_GO_REBUILD_URSELF(argc, argv);

  if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;

  Nob_Cmd cmd = {0};

  nob_cmd_append(&cmd, "cc", "-I./"SRC_DIR, "-I./"ASSETS_DIR);
  nob_cmd_append(&cmd, "-Wall", "-Wextra", "-fsanitize=address");

  if (!is_library_installed("raylib")) {
    if (access("build/raylib-5.5_linux_amd64", F_OK) != 0) {
      if (!is_program_installed("curl")) {
	fprintf(stderr, "ERROR: cannot fetch the raylib as curl is not installed\n");
	return 1;
      }

      int curl_res;
      if ((curl_res = system("curl -L -o build/raylib-5.5_linux_amd64.tar.gz https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz")) != 0) {
	fprintf(stderr, "ERROR: failed to curl the raylib\n");
	return 1;
      }

      int tar_res;
      if (tar_res = system("tar xvf build/raylib-5.5_linux_amd64.tar.gz -C build/") != 0) {
	fprintf(stderr, "ERROR: failed to tar xvf the raylib\n");
	return 1;
      }
    }

    nob_cmd_append(&cmd, "build/raylib-5.5_linux_amd64/lib/libraylib.a");
    nob_cmd_append(&cmd, "-I./build/raylib-5.5_linux_amd64/include");
  } else {
    nob_cmd_append(&cmd, "-lraylib");
  }

  nob_cmd_append(&cmd, "-o", BUILD_DIR""CLIENT_EXE_NAME, SRC_DIR"main_client.c");
  nob_cmd_append(&cmd, SRC_DIR"client.c", SRC_DIR"input.c", SRC_DIR"vec2.c", SRC_DIR"game_state.c", SRC_DIR"button.c");
  nob_cmd_append(&cmd, "-lm");

  if (!is_library_installed("raylib")) {
    nob_cmd_append(&cmd, "build/raylib-5.5_linux_amd64/lib/libraylib.a");
    nob_cmd_append(&cmd, "-lGL", "-lpthread", "-ldl", "-lX11");
  } else {
    nob_cmd_append(&cmd, "-lraylib");
  }
  
  if (!nob_cmd_run(&cmd)) return 1;

  nob_cmd_append(&cmd, "cc", "-I./"SRC_DIR, "-I./"ASSETS_DIR);
  nob_cmd_append(&cmd, "-Wall", "-Wextra", "-fsanitize=address");

  nob_cmd_append(&cmd, "-o", BUILD_DIR""SERVER_EXE_NAME);
  nob_cmd_append(&cmd, SRC_DIR"main_server.c", SRC_DIR"server.c", SRC_DIR"game_state.c");

  nob_cmd_append(&cmd, "-lm");

  if (!is_library_installed("raylib")) {
    nob_cmd_append(&cmd, "build/raylib-5.5_linux_amd64/lib/libraylib.a");
    nob_cmd_append(&cmd, "-lGL", "-lpthread", "-ldl", "-lX11");
  } else {
    nob_cmd_append(&cmd, "-lraylib");
  }

  if (!nob_cmd_run(&cmd)) return 1;
  return 0;
}

bool is_program_installed(const char *name) {
  if (!name) {
    return false;
  }

  size_t len = strlen(name);

  if (len == 0) {
    return false;
  }

  char *which_command = calloc(5 + 1 + strlen(name) + 1, sizeof(char));
  if (!which_command) {
    fprintf(stderr, "ERROR: failed to allocate data for which command\n");
    return false;
  }

  sprintf(which_command, "which %s", name);

  int res = system(which_command);
  free(which_command);
  return res == 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

bool is_library_installed(const char *name) {
    if (!name || strlen(name) == 0) {
        return false;
    }

    char libname[256];
    if (snprintf(libname, sizeof(libname), "lib%s.so", name) >= sizeof(libname)) {
        return false;
    }

    const char *lib_path = getenv("LD_LIBRARY_PATH");
    if (!lib_path) {
        return false;
    }

    char *path_copy = strdup(lib_path);
    if (!path_copy) {
        return false;
    }

    bool found = false;
    char *dir = strtok(path_copy, ":");
    while (dir && !found) {
        char full_path[1024];
        if (snprintf(full_path, sizeof(full_path), "%s/%s", dir, libname) < sizeof(full_path)) {
            if (access(full_path, F_OK) == 0) {
                found = true;
            }
        }
        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return found;
}
