#include <cstdint>

#include "game/game.h"
#include "input/input_handler.h"

int32_t main() {
    Game game;
    InputHandler input(game);

    while (true) {
        input.GetInput();
        game.Print();
    }

    return 0;
}
