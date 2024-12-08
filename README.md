# Tic-Tac-Toe

A C++ implementation of the classic Tic-Tac-Toe game built using modern C++
The project is organized into several components:

## Project Structure

The project is organized into several components:
- `bin/` - Contains the main executable
- `lib/` - Contains the core game libraries:
  - `board/` - Board representation and logic
  - `game/` - Game mechanics and rules
  - `input/` - User input handling
- `tests/` - Unit tests (optional)

## Prerequisites

- CMake (minimum version 3.5.0)
- C++ 23 compatible compiler
- Build tools (Make, etc.)

## Build Instructions

Create a build directory:
```bash
mkdir build && cd build
```
Generate build files:
```bash
cmake ..
```

Build the project:
```bash
cmake --build .
```

To enable tests, build with:

cmake -DBUILD_TESTS=ON ..
