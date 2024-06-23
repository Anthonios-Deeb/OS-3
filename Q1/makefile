CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic -O3

all: main

main: main.o kosaraju.o
	$(CXX) $(CXXFLAGS) -o main main.o kosaraju.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f main *.o

.PHONY: all clean