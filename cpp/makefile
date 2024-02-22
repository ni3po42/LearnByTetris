tetris: main.o generator.o events.o piece.o board.o game.o input.factory.o output.factory.o utility.o
	g++ -std=c++23 -o tetris main.o input/input.factory.o events/events.o piece/piece.o board/board.o game/game.o output/output.factory.o utility.o -lncurses -lpthread -D_TETRIS_IO_CURSES

generator.o:
	(cd generator && make)

main.o: main.cpp
	g++ -std=c++23 -g -O -c main.cpp

events.o:
	(cd events && make)

piece.o:
	(cd piece && make)

board.o:
	(cd board && make)

game.o:
	(cd game && make)

input.factory.o:
	(cd input && make)

output.factory.o:
	(cd output && make)

utility.o: utility.cpp utility.hpp
	g++ -std=c++23 -g -O -c utility.cpp
