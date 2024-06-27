#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include <sstream>
#include <cstring>
#include "vectorList_kosaraju.hpp"
#include "packets.hpp"
#include "reactor_proactor.hpp"

#define PORT 9034

using namespace std;

// graph
vectorList_kosaraju *graph = nullptr;
pthread_mutex_t clientMutex;


vector<string> splitString(const string &input)
{
    vector<string> result;

    // Check if the input contains spaces or commas
    if (input.find(' ') != string::npos || input.find(',') != string::npos)
    {
        istringstream stream(input);
        string part;

        // First split by space to separate the command from the numbers
        while (getline(stream, part, ' '))
        {
            result.push_back(part);
        }

        // If there is a comma-separated part, split that too
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
        // If there are no spaces or commas, just add the input to the result
        result.push_back(input);
    }

    return result;
}

void processClientRequest(op_Packet p, int client_socket)
{
    if (p.operation == "newgraph")
    {
        pthread_mutex_lock(&clientMutex);
        string message = "Creating graph with " + to_string(p.v1) + " nodes and " + to_string(p.v2) + " edges";
        cout << message << endl;
        send(client_socket, message.c_str(), message.size() + 1, 0);
        // receive edges packet
        char buffer[1024] = {0};
        if (recv(client_socket, buffer, 1024, 0) <= 0)
        {
            perror("Read failed");
            close(client_socket);
            pthread_mutex_unlock(&clientMutex);
            return;
        }
        edges_Packet edges;
        deserializeEdgesPacket(buffer, edges);

        // create graph
        graph = new vectorList_kosaraju(p.v1, p.v2, edges.edges);
        send(client_socket, "Graph created", 15, 0);
        cout << "Graph created" << endl;
        pthread_mutex_unlock(&clientMutex);
    }
    else if (p.operation == "kosaraju")
    {
        pthread_mutex_lock(&clientMutex);
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
        pthread_mutex_unlock(&clientMutex);
    }
    else if (p.operation == "newedge")
    {
        pthread_mutex_lock(&clientMutex);
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
        pthread_mutex_unlock(&clientMutex);
    }
    else if (p.operation == "removeedge")
    {
        pthread_mutex_lock(&clientMutex);
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
        pthread_mutex_unlock(&clientMutex);
    }
    else if (p.operation == "exit")
    {
        send(client_socket, "Connection terminated", 22, 0);
        close(client_socket);
        pthread_mutex_unlock(&clientMutex);
        return;
    }
    else
    {
        cerr << "Invalid command" << endl;
        send(client_socket, "Invalid command", 16, 0);
        pthread_mutex_unlock(&clientMutex);
    }
}

void handleClient(int client_socket)
{
    char buffer[1024] = {0};
    while (true)
    {
        int recv_val = recv(client_socket, buffer, 1024, 0);
        if (recv_val < 0)
        {
            perror("Read failed");
            close(client_socket);
            return;
        }
        else if (recv_val == 0)
        {
            cout << "Client disconnected" << endl;
            close(client_socket);
            return;
        }
        else
        {
            op_Packet p;
            deserializeOpPacket(buffer, p);
            processClientRequest(p, client_socket);
        }
    }
}

// Proactor function
void* proactorThreadFunc(int client_socket) {
    handleClient(client_socket);
    return nullptr;
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int addrlen = sizeof(server_address);

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // zero out the struct
    memset(&server_address, '0', sizeof(server_address));
    memset(&client_address, '0', sizeof(client_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    cout << "Listening on port " << PORT << endl;
    if (listen(server_socket, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "Waiting for connections..." << endl;

    // Initialize mutex
    pthread_mutex_init(&clientMutex, nullptr);

    // Accept incoming connections
    while (true) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        cout << "New connection" << endl;

        // Start a new proactor thread to handle the client connection
        pthread_t tid = startProactor(client_socket, proactorThreadFunc);
        if (tid == 0) {
            perror("Failed to create proactor thread");
            close(client_socket);
            continue;
        }

        // Detach the thread
        pthread_detach(tid);
    }

    close(server_socket);
    return 0;
}