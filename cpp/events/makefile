events.o: events.hpp events.cpp
	g++ -std=c++23 -g -O -c events.cpp

test: events.hpp events.cpp test.cpp
	g++ -std=c++23 -g -o events.test events.cpp test.cpp
	./events.test

