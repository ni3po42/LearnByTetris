board.o: board.hpp board.cpp
	g++ -std=c++23 -g -O -c board.cpp

test: board.hpp board.cpp test.cpp
	g++ -std=c++23 -g -o board.test test.cpp board.cpp
	./board.test

