#valgrind --tool=memcheck --track-origins=yes --leak-check=yes --show-reachable=yes -v $cmd
valgrind --tool=memcheck --track-origins=yes --leak-check=yes --show-reachable=yes ./run
