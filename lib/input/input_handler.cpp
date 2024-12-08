#include "input_handler.h"

#include <cstddef>
#include <iostream>

InputHandler::InputHandler(Game &game) : game(game) {}

void InputHandler::GetInput() {
    Game::Turn turn = game.GetTurn();

    size_t x;
    size_t y;

    switch (turn) {
        case Game::Circle:
            std::cout << "\033[1;31mCircle Input:\033[0m\n";
            std::cin >> x >> y;

            if (x >= game.GetSide() || y >= game.GetSide()) {
                return;
            }

            game.SetCellOwner(y, x);
            break;

        case Game::Cross:
            std::cout << "\033[1;31mCross Input:\033[0m\n";
            std::cin >> x >> y;

            if (x >= game.GetSide() || y >= game.GetSide()) {
                return;
            }

            game.SetCellOwner(y, x);
            break;
    }
}
