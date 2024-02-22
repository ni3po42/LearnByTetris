piece.o: piece.hpp piece.cpp
	g++ -std=c++23 -g -O -c piece.cpp

test: piece.hpp piece.cpp test.cpp
	g++ -std=c++23 -g -o piece.test test.cpp piece.cpp ../utility.cpp
	./piece.test

