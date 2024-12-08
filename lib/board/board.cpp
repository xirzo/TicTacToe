
#include "board.h"

#include <cstddef>
#include <iostream>

#include "cell.h"

Board::Board() {
    grid = new Cell*[SIDE];

    for (size_t i = 0; i < SIDE; ++i) {
        grid[i] = new Cell[SIDE];
    }
}

Board::~Board() {
    for (size_t i = 0; i < SIDE; ++i) {
        delete[] grid[i];
    }

    delete[] grid;
}

void Board::SetCellOwner(size_t x, size_t y, Cell::Fill owner) {
    grid[x][y].SetOwner(owner);
}

bool Board::CanSetCellOwner(size_t x, size_t y) {
    return grid[x][y].GetOwner() == Cell::Empty;
}

void Board::Reset() {
    for (size_t i = 0; i < SIDE; ++i) {
        for (size_t j = 0; j < SIDE; ++j) {
            grid[i][j].Clear();
        }
    }
}

void Board::Print() {
    for (size_t i = 0; i < SIDE; ++i) {
        for (size_t j = 0; j < SIDE; ++j) {
            std::cout << grid[i][j].GetOwner();
        }

        std::cout << std::endl;
    }
}

bool Board::HasAnyoneWon() {
    for (size_t i = 0; i < 3; ++i) {
        if (grid[i][0].GetOwner() != Cell::Empty &&
            grid[i][0].GetOwner() == grid[i][1].GetOwner() &&
            grid[i][1].GetOwner() == grid[i][2].GetOwner()) {
            return true;
        }
    }

    for (size_t j = 0; j < 3; ++j) {
        if (grid[0][j].GetOwner() != Cell::Empty &&
            grid[0][j].GetOwner() == grid[1][j].GetOwner() &&
            grid[1][j].GetOwner() == grid[2][j].GetOwner()) {
            return true;
        }
    }

    if (grid[0][0].GetOwner() != Cell::Empty &&
        grid[0][0].GetOwner() == grid[1][1].GetOwner() &&
        grid[1][1].GetOwner() == grid[2][2].GetOwner()) {
        return true;
    }

    if (grid[0][2].GetOwner() != Cell::Empty &&
        grid[0][2].GetOwner() == grid[1][1].GetOwner() &&
        grid[1][1].GetOwner() == grid[2][0].GetOwner()) {
        return true;
    }

    return false;
}
