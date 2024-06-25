// reactor.hpp

#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <functional>
#include <unordered_map>
#include <vector>
#include <poll.h>

using namespace std;

// Define reactorFunc as a function pointer type
typedef void * (*reactorFunc)(int fd);

class Reactor {
public:
    // Starts a new reactor and returns a pointer to it
    static Reactor * startReactor();

    // Adds fd to Reactor (for reading); returns 0 on success.
    int addFdToReactor(int fd, reactorFunc func);

    // Removes fd from reactor
    int removeFdFromReactor(int fd);

    // Stops reactor
    int stopReactor();

    // Run the reactor loop
    void run();

private:
    Reactor();
    ~Reactor();
    
    vector<struct pollfd> poll_fds;
    unordered_map<int, reactorFunc> fd_map;
    bool running;
};

#endif // REACTOR_HPP
