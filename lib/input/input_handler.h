#pragma once

#include "game.h"

class InputHandler {
public:
    InputHandler(Game &game);

    void GetInput();

private:
    Game &game;
};
