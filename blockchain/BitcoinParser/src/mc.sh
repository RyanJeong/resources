#!/bin/bash
echo "Command: ./run getblock <INDEX>, ./run tx <TXID> ./run address"
read cmd
#valgrind --tool=memcheck --track-origins=yes --leak-check=yes --show-reachable=yes -v $cmd
valgrind --tool=memcheck --track-origins=yes --leak-check=yes --show-reachable=yes $cmd
