#include "cell.h"

Cell::Cell() : fill(Empty) {}

void Cell::SetOwner(Fill owner) {
    fill = owner;
}

Cell::Fill Cell::GetOwner() {
    return fill;
}

void Cell::Clear() {
    fill = Empty;
}
