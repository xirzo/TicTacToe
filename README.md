# TicTacToe

A C multiplayer implementation of the Tic-Tac-Toe game. Includes a server and a client.

## Build Instructions

Create a build directory:

```bash
mkdir build && cd build
```

Generate build files:

```bash
cmake .. -DFETCH_LIBS=1
```

or

```bash
# (!IMPORTANT) you should have [raylib](https://github.com/raysan5/raylib) installed on your computer

cmake .. -DFETCH_LIBS=0
```

Build the project:

```bash
cmake --build .
```
