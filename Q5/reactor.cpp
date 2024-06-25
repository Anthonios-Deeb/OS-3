// reactor.cpp

#include "reactor.hpp"
#include <unistd.h>
#include <iostream>
#include <algorithm>

Reactor::Reactor() : running(false) {}

Reactor::~Reactor() {
    for (const auto &pfd : poll_fds) {
        close(pfd.fd);
    }
}

Reactor * Reactor::startReactor() {
    return new Reactor();
}

int Reactor::addFdToReactor(int fd, reactorFunc func) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    poll_fds.push_back(pfd);
    fd_map[fd] = func;
    return 0;
}

int Reactor::removeFdFromReactor(int fd) {
    poll_fds.erase(std::remove_if(poll_fds.begin(), poll_fds.end(), [fd](const struct pollfd &pfd) {
        return pfd.fd == fd;
    }), poll_fds.end());
    fd_map.erase(fd);
    return 0;
}

int Reactor::stopReactor() {
    running = false;
    return 0;
}

void Reactor::run() {
    running = true;
    while (running) {
        int activity = poll(poll_fds.data(), poll_fds.size(), -1);

        if (activity < 0 && errno != EINTR) {
            cerr << "poll error" << endl;
        }

        for (const auto &pfd : poll_fds) {
            if (pfd.revents & POLLIN) {
                auto it = fd_map.find(pfd.fd);
                if (it != fd_map.end()) {
                    reactorFunc func = it->second;
                    func(pfd.fd);
                }
            }
        }
    }
}
