CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic -O3 -g -pg

all: deque_main list_main

deque_main: deque_main.o deque_kosaraju.o 
	$(CXX) $(CXXFLAGS) -o deque_main deque_main.o deque_kosaraju.o

list_main: list_main.o list_kosaraju.o 
	$(CXX) $(CXXFLAGS) -o list_main list_main.o list_kosaraju.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o deque_main list_main

.PHONY: all clean