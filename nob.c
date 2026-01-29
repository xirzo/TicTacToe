#define NOB_IMPLEMENTATION
#include "nob.h"

#define CLIENT_EXE_NAME "tictactoe_client"
#define SERVER_EXE_NAME "tictactoe_server"

#define ASSETS_DIR "assets/"
#define BUILD_DIR "build/"
#define SRC_DIR "src/"

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return 1;

    Nob_Cmd cmd = {0};

    // TODO: fetch raylib if not present
    nob_cmd_append(&cmd, "cc", "-I./"SRC_DIR, "-I./"ASSETS_DIR);
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-fsanitize=address", "-lraylib", "-lm");
    nob_cmd_append(&cmd, "-o", BUILD_DIR""CLIENT_EXE_NAME, SRC_DIR"main_client.c");
    nob_cmd_append(&cmd, SRC_DIR"client.c", SRC_DIR"input.c", SRC_DIR"vec2.c", SRC_DIR"game_state.c", SRC_DIR"button.c");

    if (!nob_cmd_run(&cmd)) return 1;

    nob_cmd_append(&cmd, "cc", "-I./"SRC_DIR, "-I./"ASSETS_DIR);
    nob_cmd_append(&cmd, "-Wall", "-Wextra", "-fsanitize=address", "-lraylib", "-lm");
    nob_cmd_append(&cmd, "-o", BUILD_DIR""SERVER_EXE_NAME, SRC_DIR"main_server.c");
    nob_cmd_append(&cmd, SRC_DIR"server.c", SRC_DIR"game_state.c");

    if (!nob_cmd_run(&cmd)) return 1;
    return 0;
}
