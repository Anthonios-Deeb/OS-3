#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <functional>
#include <vector>
#include <poll.h>
#include <unordered_map>

using namespace std;

// Typedef for reactor function pointer
typedef void* (*reactorFunc)(int fd);

class ReactorImpl {
public:
    ReactorImpl();

    int addFd(int fd, reactorFunc func);

    int removeFd(int fd);

    void run();

private:
    vector<pollfd> pollfds;
    unordered_map<int, reactorFunc> handlers;
};

// Function to start a new reactor and return pointer to it
void* startReactor();

// Function to add fd to Reactor (for reading); returns 0 on success
int addFdToReactor(void* reactor, int fd,reactorFunc func);

// Function to remove fd from reactor
int removeFdFromReactor(void* reactor, int fd);

// Function to stop reactor
int stopReactor(void* reactor);

// Function to run reacto
int runReactor(void* reactor);

#endif // REACTOR_HPP
