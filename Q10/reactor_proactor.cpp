#include "reactor_proactor.hpp"


ReactorImpl::ReactorImpl(){};

int ReactorImpl::addFd(int fd, reactorFunc func)
{
    pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pollfds.push_back(pfd);
    handlers[fd] = func;
    return 0; // Success
}

int ReactorImpl::removeFd(int fd)
{
    auto it = std::find_if(pollfds.begin(), pollfds.end(), [fd](const pollfd &pfd)
                           { return pfd.fd == fd; });
    if (it != pollfds.end())
    {
        pollfds.erase(it);
        handlers.erase(fd);
        return 0; // Success
    }
    return -1; // Not found
}

void ReactorImpl::run()
{
    const int TIMEOUT_MS = -1; // Wait indefinitely
    while (true)
    {
        int numEvents = poll(pollfds.data(), pollfds.size(), TIMEOUT_MS);
        if (numEvents == -1)
        {
            throw std::runtime_error("poll failed");
        }

        for (auto &pfd : pollfds)
        {
            if (pfd.revents & POLLIN)
            {
                handlers[pfd.fd](pfd.fd);
            }
        }
    }
}

// Function to start a new proactor and return pthread ID
pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t proactorThread;
    ProactorThreadArgs *args = new ProactorThreadArgs{sockfd, threadFunc};

    if (pthread_create(&proactorThread, nullptr, [](void *arg) -> void* {
        ProactorThreadArgs *args = static_cast<ProactorThreadArgs*>(arg);
        args->threadFunc(args->sockfd);
        delete args;
        return nullptr;
    }, args) != 0) {
        perror("pthread_create failed");
        delete args;
        return 0;
    }

    return proactorThread;
}

// Function to stop proactor by pthread ID
int stopProactor(pthread_t tid)
{
    pthread_cancel(tid);
    pthread_join(tid, nullptr);
    return 0; // Success
}

// Function to start a new reactor and return pointer to it
void *startReactor()
{
    ReactorImpl *reactor = new ReactorImpl();
    return reinterpret_cast<void *>(reactor);
}

// Function to add fd to Reactor (for reading); returns 0 on success
int addFdToReactor(void *reactor, int fd, reactorFunc func)
{
    ReactorImpl *impl = reinterpret_cast<ReactorImpl *>(reactor);
    if (impl)
    {
        return impl->addFd(fd, func);
    }
    return -1; // Invalid reactor pointer
}

// Function to remove fd from reactor
int removeFdFromReactor(void *reactor, int fd)
{
    ReactorImpl *impl = reinterpret_cast<ReactorImpl *>(reactor);
    if (impl)
    {
        return impl->removeFd(fd);
    }
    return -1; // Invalid reactor pointer
}

// Function to stop reactor
int stopReactor(void *reactor)
{
    ReactorImpl *impl = reinterpret_cast<ReactorImpl *>(reactor);
    if (impl)
    {
        delete impl;
        return 0; // Success
    }
    return -1; // Invalid reactor pointer
}

// Function to run reactor
int runReactor(void *reactor)
{
    ReactorImpl *impl = reinterpret_cast<ReactorImpl *>(reactor);
    if (impl)
    {
        impl->run();
        return 0; // Success
    }

    return -1; // Invalid reactor pointer
}
