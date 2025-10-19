# TicTacToe

<img width="1788" height="589" alt="tic_tac_toe" src="https://github.com/user-attachments/assets/0359f52e-8939-43b2-ae68-f6bbb7460f98" />

A multiplayer implementation of the Tic-Tac-Toe. Includes a **server** and a **client**.

## Building & Running

> [!NOTE] 
> In order to use `FETCH_LIBS=OFF` you must have libraries installed of your machine.
>
> Also if you want to run debug mode, turn it on via `cmake -B build/ -DCMAKE_BUILD_TYPE=Debug` overwise  `cmake -B build/ -DCMAKE_BUILD_TYPE=Release`

```sh
cmake -DFETCH_LIBS=ON -B build/
cmake --build build/
```

Now enter `build/bin` directory and use *start.sh* script there or manually run server and clients

```sh
./start.sh
```
