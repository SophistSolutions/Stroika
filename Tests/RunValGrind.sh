!/bin/sh
valgrind --track-origins=yes --tool=memcheck --leak-check=yes $1
