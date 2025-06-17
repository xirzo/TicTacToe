#!/bin/sh

make

./tic_tac_toe_server &
SERVER_PID=$!

./tic_tac_toe

kill $SERVER_PID
echo "Server stopped."
