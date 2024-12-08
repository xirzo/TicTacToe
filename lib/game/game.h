#pragma once

#include "board.h"

class Game {
public:
    enum Turn {
        Circle = 1,
        Cross = 2,
    };

    Game();

    void Start();
    void Stop();
    void Print();
    void SetCellOwner(size_t x, size_t y);
    Turn GetTurn();
    const size_t GetSide() const;

private:
    Board board;
    bool has_started;
    Turn turn;
};
