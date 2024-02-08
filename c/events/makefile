events.o: events.c events.h events.internal.h
	gcc -g -O -c events.c
	
test: events.c events.h events.internal.h events.test.c
	gcc -g -o events.test events.test.c events.c "../generator/generator.c" -lpthread
	./events.test