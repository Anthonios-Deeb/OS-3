#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#include <cstring>
#include "vectorList_kosaraju.hpp"
#include "packets.hpp"
#include "reactor.hpp"

#define PORT 9034

using namespace std;

void *reactor = startReactor();


// Graph
vectorList_kosaraju *graph = nullptr;

vector<string> splitString(const string &input)
{
    vector<string> result;
    if (input.find(' ') != string::npos || input.find(',') != string::npos)
    {
        istringstream stream(input);
        string part;
        while (getline(stream, part, ' '))
        {
            result.push_back(part);
        }
        if (result.size() > 1)
        {
            string numbers = result.back();
            result.pop_back();
            istringstream numStream(numbers);
            while (getline(numStream, part, ','))
            {
                result.push_back(part);
            }
        }
    }
    else
    {
        result.push_back(input);
    }
    return result;
}

void processClientRequest(op_Packet p, int client_socket)
{
    if (p.operation == "newgraph")
    {
        string message = "Creating graph with " + to_string(p.v1) + " nodes and " + to_string(p.v2) + " edges";
        cout << message << endl;
        send(client_socket, message.c_str(), message.size() + 1, 0);
        char buffer[1024] = {0};
        if (recv(client_socket, buffer, 1024, 0) <= 0)
        {
            perror("Read failed");
            close(client_socket);
            return;
        }
        edges_Packet edges;
        deserializeEdgesPacket(buffer, edges);
        graph = new vectorList_kosaraju(p.v1, p.v2, edges.edges);
        send(client_socket, "Graph created", 15, 0);
        cout << "Graph created" << endl;
    }
    else if (p.operation == "kosaraju")
    {
        if (graph == nullptr)
        {
            send(client_socket, "Graph not created", 18, 0);
        }
        else
        {
            vector<list<int>> scc = graph->findSCC();
            string result = "Strongly connected components: \n";
            for (auto &component : scc)
            {
                for (auto &node : component)
                {
                    result += to_string(node) + " ";
                }
                result += "\n";
            }
            send(client_socket, result.c_str(), result.size() + 1, 0);
        }
    }
    else if (p.operation == "newedge")
    {
        if (graph == nullptr)
        {
            send(client_socket, "Graph not created", 18, 0);
        }
        else
        {
            int result = graph->addEdge(p.v1, p.v2);
            if (result == 1)
            {
                send(client_socket, "Edge added", 11, 0);
            }
            else if (result == 0)
            {
                send(client_socket, "Edge already exists", 20, 0);
            }
            else
            {
                send(client_socket, "Invalid node", 13, 0);
            }
        }
    }
    else if (p.operation == "removeedge")
    {
        if (graph == nullptr)
        {
            send(client_socket, "Graph not created", 18, 0);
        }
        else
        {
            int result = graph->removeEdge(p.v1, p.v2);
            if (result == 1)
            {
                send(client_socket, "Edge removed", 13, 0);
            }
            else if (result == 0)
            {
                send(client_socket, "Edge does not exist", 20, 0);
            }
            else
            {
                send(client_socket, "Invalid node", 13, 0);
            }
        }
    }
    else if (p.operation == "exit")
    {
        send(client_socket, "Connection terminated", 22, 0);
        close(client_socket);
    }
    else
    {
        cerr << "Invalid command" << endl;
        send(client_socket, "Invalid command", 16, 0);
    }
}

void *handleClient(int client_socket)
{
    char buffer[1024] = {0};
    int received = recv(client_socket, buffer, 1024, 0);
    if (received < 0)
    {
        perror("Read failed");
        close(client_socket);
    }
    else if (received == 0)
    {
        cout << "Client disconnected" << endl;
        removeFdFromReactor(reactor,client_socket);
        close(client_socket);
    }
    else
    {
        op_Packet p;
        deserializeOpPacket(buffer, p);
        processClientRequest(p, client_socket);
    }

    return nullptr;
}

int main()
{
    int server_socket;
    struct sockaddr_in server_address;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    cout << "Listening on port " << PORT << endl;
    if (listen(server_socket, 3) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }


    addFdToReactor(reactor,server_socket, [](int fd) -> void* {
        struct sockaddr_in client_address;
        int client_socket;
        int addrlen = sizeof(client_address);
        if ((client_socket = accept(fd, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            close(fd);
            return nullptr; // or whatever your function needs to return
        }
        cout << "New connection" << endl;

        addFdToReactor(reactor,client_socket, handleClient);

        return nullptr; // or whatever your function needs to return
    });

    runReactor(reactor);
    close(server_socket);
    return 0;
}