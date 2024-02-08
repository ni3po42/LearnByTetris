board.o: board.c board.h
	gcc -g -O -c board.c
	
piece.test.o: board.h board.test.c
	gcc -g -O -c board.test.c -lpthread
	
test: board.o board.test.o generator.o
	gcc -o board.test ../generator/generator.o board.o board.test.o -lpthread
	./board.test
	
generator.o:
	(cd ../generator && make)
	
