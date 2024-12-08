#pragma once

#include <cstddef>

#include "cell.h"

class Board {
public:
    const size_t SIDE = 3;

    Board();
    ~Board();

    void SetCellOwner(size_t x, size_t y, Cell::Fill owner);
    bool CanSetCellOwner(size_t x, size_t y);
    void Reset();
    void Print();

    bool HasAnyoneWon();

private:
    Cell** grid;
};
