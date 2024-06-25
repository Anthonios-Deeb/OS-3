#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sstream>
#include "../Q3/vectorList_kosaraju.hpp"

#define PORT 9034

using namespace std;

int main()
{

    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int addrlen = sizeof(server_address);
    fd_set readfds, masterfds;

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    // if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    // {
    //     perror("Bind failed");
    //     exit(EXIT_FAILURE);
    // }

    return 0;
}