piece.o: piece.c piece.h
	gcc -g -O -c piece.c

piece.test.o: piece.h piece.test.c
	gcc -g -O -c piece.test.c -lpthread
	
test: piece.o piece.test.o generator.o
	gcc -o piece.test ../generator/generator.o piece.o piece.test.o -lpthread
	./piece.test
	
generator.o:
	(cd ../generator && make)
