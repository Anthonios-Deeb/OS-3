CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic -O3 -g -pg

all: vectorList_main matrix_main

vectorList_main: vectorList_main.o vectorList_kosaraju.o
	$(CXX) $(CXXFLAGS) -o vectorList_main vectorList_main.o vectorList_kosaraju.o

matrix_main: matrix_main.o matrix_kosaraju.o
	$(CXX) $(CXXFLAGS) -o matrix_main matrix_main.o matrix_kosaraju.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o vectorList_main  matrix_main

.PHONY: all clean