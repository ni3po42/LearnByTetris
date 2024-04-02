generator.o: generator.c generator.h generator.internal.h
	gcc -g -O -c generator.c
	
test: generator.c generator.h generator.internal.h generator.test.c
	gcc -g -o generator.tests generator.test.c generator.c -lpthread
	./generator.tests