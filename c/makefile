tetris: main.o events.o generator.o piece.o board.o game.o input.o render.o utilities.o
	gcc -o tetris main.o input/input.o events/events.o generator/generator.o piece/piece.o board/board.o game/game.o render/render.o utilities.o -lncurses -lpthread

it: tetris
	./tetris

main.o: main.c
	gcc -g -O -c main.c

events.o:
	(cd events && make)
	
generator.o:
	(cd generator && make)
	
piece.o:
	(cd piece && make)
	
board.o:
	(cd board && make)
	
game.o:
	(cd game && make)
	
input.o:
	(cd input && make)
	
render.o:
	(cd render && make)

utilities.o: utilities.c utilities.h
	gcc -g -O -c utilities.c
	
test:
	(cd events && make test)
	(cd generator && make test)
	(cd board && make test)
	(cd piece && make test)
