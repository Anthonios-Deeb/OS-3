CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O3

# Source files
SERVER_SRCS = server.cpp vectorList_kosaraju.cpp reactor.cpp packets.cpp
CLIENT_SRCS = client.cpp packets.cpp

# Object files
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o server client

.PHONY: all clean