#include "game.h"

#include <cstdlib>
#include <iostream>

Game::Game() : has_started(false), turn(Circle) {}

void Game::Start() {
    if (has_started == false) {
        board.Reset();
    }
}

void Game::Stop() {
    if (has_started == true) {
        has_started = false;
    }
}

void Game::Print() {
    board.Print();
}

void Game::SetCellOwner(size_t x, size_t y) {
    if (board.CanSetCellOwner(x, y) == false) {
        return;
    }

    board.SetCellOwner(x, y, static_cast<Cell::Fill>(turn));

    if (board.HasAnyoneWon()) {
        switch (turn) {
            case Circle:
                std::cout << "\033[1;31mCircle won!\033[0m\n";
                break;
            case Cross:
                std::cout << "\033[1;31mCross won!\033[0m\n";
                break;
        }

        exit(0);
    }

    switch (turn) {
        case Circle:
            turn = Cross;
            break;
        case Cross:
            turn = Circle;
            break;
    }
}

Game::Turn Game::GetTurn() {
    return turn;
}

const size_t Game::GetSide() const {
    return board.SIDE;
}
