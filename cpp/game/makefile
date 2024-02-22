events.o: game.hpp game.cpp
	g++ -std=c++23 -g -O -c game.cpp

test: game.hpp game.cpp test.cpp
	g++ -std=c++23 -g -o game.test game.cpp test.cpp
	./game.test

