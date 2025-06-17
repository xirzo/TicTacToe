# TicTacToe

A C multiplayer implementation of the Tic-Tac-Toe game. Includes a server and a client.

![image](https://github.com/user-attachments/assets/7360d829-a9dc-48a0-b81b-d6062b25f887)

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
