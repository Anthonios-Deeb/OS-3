
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
#include "vectorList_kosaraju.hpp"

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

void createGraph(int client_socket, int n, int m)
{
    send(client_socket, "Creating graph...", 18, 0);

    vector<list<int>> adjList(m);

    for (int i = 0; i < m; i++)
    {
        int u, v;
        send(client_socket, "Enter edge (u, v): ", 20, 0);
        recv(client_socket, &u, sizeof(u), 0);
        recv(client_socket, &v, sizeof(v), 0);

        // check if the edge is valid
        list<int> temp = {u,v};
        if (u < 1 || u > n || v < 1 || v > n || u == v)
        {
            send(client_socket, "Invalid edge\n", 14, 0);
            i--;
            continue;
        }
        // check if the edge already exists
        else if (find(adjList.begin(), adjList.end(), temp) != adjList.end())
        {
            send(client_socket, "Edge already exists\n", 21, 0);
            i--;
            continue;
        }

        adjList.push_back({u, v});
    }

    graph = new vectorList_kosaraju(n, m, adjList);
}

void processClientRequest(char *buffer, int client_socket)
{
    if (buffer == NULL)
    {
        cout << "Buffer is null" << endl;
        return;
    }

    string str = buffer;
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    vector<string> tokens = splitString(str);

    if (tokens[0] == "newgraph" && tokens.size() == 3)
    {
        try
        {
            int n = stoi(tokens[1]);
            int m = stoi(tokens[2]);
            createGraph(client_socket, n, m);
            send(client_socket, "Graph created", 14, 0);
        }
        catch (const std::exception &e)
        {
            send(client_socket, "Invalid input", 14, 0);
        }
    }
    else if (tokens[0] == "kosaraju" && tokens.size() == 1)
    {
        if (graph == nullptr)
        {
            send(client_socket, "There is no Graph !!", 21, 0);
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
    else if (tokens[0] == "newedge" && tokens.size() == 3)
    {
        if (graph == nullptr)
        {
            send(client_socket, "There is no Graph !!", 21, 0);
        }
        else
        {
            try
            {
                int u = stoi(tokens[1]);
                int v = stoi(tokens[2]);
                graph->addEdge(u, v);
                send(client_socket, "Edge added", 11, 0);
            }
            catch (const exception &e)
            {
                send(client_socket, "Invalid input", 14, 0);
            }
        }
    }
    else if (tokens[0] == "removeedge" && tokens.size() == 3)
    {
        if (graph == nullptr)
        {
            send(client_socket, "Graph not created", 18, 0);
        }
        else
        {
            try
            {
                int u = stoi(tokens[1]);
                int v = stoi(tokens[2]);
                graph->removeEdge(u, v);
                send(client_socket, "Edge removed", 13, 0);
            }
            catch (const exception &e)
            {
                send(client_socket, "Invalid input", 14, 0);
            }
        }
    }
    else if (tokens[0] == "exit")
    {
        send(client_socket, "Connection terminated", 22, 0);
        close(client_socket);
        exit(1);
    }
    else
    {
        send(client_socket, "Invalid command", 16, 0);
    }
}

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
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&masterfds);
    FD_SET(server_socket, &masterfds);

    while (true)
    {
        if (select(FD_SETSIZE, &masterfds, NULL, NULL, NULL) < 0)
        {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &masterfds))
            {
                if (i == server_socket) // New connection
                {
                    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&addrlen)) < 0)
                    {
                        perror("Accept failed");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(client_socket, &masterfds);
                }
                else
                {
                    char buffer[1024] = {0};
                    int valread = read(i, buffer, 1024);
                    if (valread <= 0)
                    {
                        close(i);
                        FD_CLR(i, &masterfds);
                    }
                    else
                    {
                        processClientRequest(buffer, i);
                    }
                }
            }
        }
    }

    close(server_socket);
    return 0;
}