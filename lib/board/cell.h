#pragma once

class Cell {
public:
    enum Fill { Empty = 0, Circle = 1, Cross = 2 };

    Cell();

    void SetOwner(Fill fill);
    Fill GetOwner();
    void Clear();

private:
    Fill fill;
};
