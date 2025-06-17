#!/bin/bash

make

./tic_tac_toe_server &
SERVER_PID=$!

./tic_tac_toe &
CLIENT1_PID=$!

./tic_tac_toe &
CLIENT2_PID=$!

cleanup() {
    kill $CLIENT1_PID 2>/dev/null
    kill $CLIENT2_PID 2>/dev/null
    kill $SERVER_PID 2>/dev/null
    echo "All processes terminated."
    exit
}

trap cleanup INT TERM EXIT

wait -n

cleanup
