generator.o: generator.hpp
	echo "inclueded as header only"


test: generator.hpp test.cpp
	g++ -std=c++23 -g -O -c test.cpp
	g++ -std=c++23 -o generator.test test.o
	./generator.test