#include <vector>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sstream>
#include <cstring>
#include "vectorList_kosaraju.hpp"
#include "packets.hpp"

#define PORT 9034

using namespace std;

// graph
vectorList_kosaraju *graph = nullptr;

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
        string message = "Creating graph with " + to_string(p.v1) + " nodes and " + to_string(p.v2) + " edges";
        cout << message << endl;
        send(client_socket, message.c_str(), message.size() + 1, 0);
        // receive edges packet
        char buffer[1024] = {0};
        if (recv(client_socket, buffer, 1024, 0) <= 0)
        {
            perror("Read failed");
            exit(EXIT_FAILURE);
        }
        edges_Packet edges;
        deserializeEdgesPacket(buffer, edges);

        // create graph
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
            graph->addEdge(p.v1, p.v2);
            send(client_socket, "Edge added", 11, 0);
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
            graph->removeEdge(p.v1, p.v2);
            send(client_socket, "Edge removed", 13, 0);
        }
    }
    else if (p.operation == "exit")
    {
        send(client_socket, "Connection terminated", 22, 0);
        close(client_socket);
        exit(1);
    }
    else
    {
        cerr << "Invalid command" << endl;
        send(client_socket, "Invalid command", 16, 0);
    }
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int addrlen = sizeof(server_address);
    // fd_set masterfds;
    // int maxfds;

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

    // FD_ZERO(&masterfds);
    // FD_SET(server_socket, &masterfds);
    // maxfds = server_socket;

    cout << "Waiting for connections..." << endl;

    while (true)
    {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        cout << "New client connected" << endl;

        while (true)
        {
            // receive operation packet
            char buffer[1024] = {0};

            cout << "Waiting for operation packet" << endl;
            int recv_val = recv(client_socket, buffer, 1024, 0);
            if (recv_val < 0)
            {
                perror("Read failed");
                exit(EXIT_FAILURE);
            }
            else if (recv_val == 0)
            {
                cout << "Client disconnected" << endl;
                close(client_socket);
                break;
            }
            
            op_Packet p;
            deserializeOpPacket(buffer, p);

            cout << "Received operation: " << p.operation << endl;
            processClientRequest(p, client_socket);
        }

        break;
    }

    // while (true)
    // {
    //     cout << "Waiting for select" << endl;
    //     if (select(maxfds + 1, &masterfds, NULL, NULL, NULL) < 0)
    //     {
    //         perror("Select failed");
    //         exit(EXIT_FAILURE);
    //     }
    //     cout << "Select successful" << endl;

    //     for (int i = 0; i <= maxfds; i++)
    //     {
    //         cout << "Checking file descriptor: " << i << endl;
    //         if (FD_ISSET(i, &masterfds)) // Check if the file descriptor is set
    //         {
    //             if (i == server_socket) // New connection
    //             {
    //                 cout << "New connection" << endl;
    //                 if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0)
    //                 {
    //                     perror("Accept failed");
    //                     exit(EXIT_FAILURE);
    //                 }
    //                 FD_SET(client_socket, &masterfds);
    //                 cout << "New client connected" << endl;
    //             }
    //             else // Data from client
    //             {
    //                 char buffer[1024] = {0};
    //                 int valread = read(i, buffer, 1024);
    //                 if (valread <= 0)
    //                 {
    //                     close(i);
    //                     FD_CLR(i, &masterfds);
    //                 }
    //                 else
    //                 {
    //                     cout << "Received: " << buffer << endl;
    //                     processClientRequest(buffer, i);
    //                 }
    //             }
    //         }
    //     }
    // }

    close(server_socket);
    return 0;
}