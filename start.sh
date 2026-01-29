#!/bin/bash

make

./build/tictactoe_server &
SERVER_PID=$!

./build/tictactoe_client &
CLIENT1_PID=$!

./build/tictactoe_client &
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
