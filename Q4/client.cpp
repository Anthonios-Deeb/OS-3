#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <vector>
#include <arpa/inet.h>
#include <cstring>
#include "packets.hpp"

#define PORT 9034

using namespace std;

op_Packet parseData(string data)
{
    // split thee data into operation and values
    // data: "newgraph n,m" -> operation: "newgraph", v1: n, v2: m

    op_Packet p;

    if (data.find("newgraph") != string::npos)
    {
        p.operation = "newgraph";
        data = data.substr(9);
        p.v1 = stoi(data.substr(0, data.find(',')));
        p.v2 = stoi(data.substr(data.find(',') + 1));
    }
    else if (data.find("kosaraju") != string::npos)
    {
        p.operation = "kosaraju";
        p.v1 = -1;
        p.v2 = -1;
    }
    else if (data.find("newedge") != string::npos)
    {
        p.operation = "newedge";
        data = data.substr(8);
        p.v1 = stoi(data.substr(0, data.find(',')));
        p.v2 = stoi(data.substr(data.find(',') + 1));
    }
    else if (data.find("removeedge") != string::npos)
    {
        p.operation = "removeedge";
        data = data.substr(11);
        p.v1 = stoi(data.substr(0, data.find(',')));
        p.v2 = stoi(data.substr(data.find(',') + 1));
    }
    else if (data.find("exit") != string::npos)
    {
        p.operation = "exit";
        p.v1 = -1;
        p.v2 = -1;
    }
    else
    {   
        p.operation = "invalid";
        p.v1 = -1;
        p.v2 = -1;
    }

    return p;
}

edges_Packet createEdges(int n, int m)
{
    edges_Packet p;
    vector<list<int>> edges;

    for (int i = 0; i < m; i++)
    {
        int u, v;
        cout << "Enter edge " << i + 1 << ": ";
        cin >> u >> v;

        list<int> temp;
        temp.push_back(u);
        temp.push_back(v);

        // check if the edge is valid
        if (u < 1 || u > n || v < 1 || v > n || u == v)
        {
            cout << "Invalid edge" << endl;
            i--;
            continue;
        }

        // check if the edge already exists
        else if (find(edges.begin(), edges.end(), temp) != edges.end())
        {
            cout << "Edge already exists" << endl;
            i--;
            continue;
        }

        edges.push_back(temp);
    }

    p.n = n;
    p.m = m;
    p.edges = edges;

    return p;
}

int main()
{
    int clientSocket;
    sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Error establishing socket..." << endl;
        exit(1);
    }
    cout << "Client Socket created..." << endl;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Error connecting to server..." << endl;
        exit(1);
    }

    cout << "---Graph Operations---" << endl;
    cout << "Enter one of the following commands: " << endl;
    cout << "newgraph n,m" << endl;
    cout << "kosaraju" << endl;
    cout << "newedge u,v" << endl;
    cout << "removeedge u,v" << endl;
    cout << "exit\n"
         << endl;

    int sent, valrecv;
    while (true)
    {
        string data;
        getline(cin, data);

        transform(data.begin(), data.end(), data.begin(), ::tolower);

        op_Packet p = parseData(data);

        cout << "Operation: " << p.operation << endl;
        cout << "v1: " << p.v1 << endl;
        cout << "v2: " << p.v2 << endl;
        cout << "packet size: " << sizeof(p) << endl
             << endl;

        if (p.operation == "invalid")
        {
            cout << "Invalid command" << endl;
            continue;
        }
        else if (p.operation == "exit")
        {
            cout << "Connection terminated" << endl;
            close(clientSocket);
            exit(1);
        }

        // if operation is newgraph, send the packet
        if (p.operation == "newgraph")
        {
            vector<char> sp;
            serializeOpPacket(p, sp);
            sent = send(clientSocket, sp.data(), sp.size(), 0);
            if (sent < 0)
            {
                cerr << "Error sending data..." << endl;
                exit(1);
            }
            valrecv = recv(clientSocket, buffer, 1024, 0);
            if (valrecv < 0)
            {
                cerr << "Error receiving data..." << endl;
                exit(1);
            }
            else if (valrecv == 0)
            {
                cout << "Connection closed by server" << endl;
                close(clientSocket);
                exit(1);
            }
            cout << buffer << endl;

            // create edges
            edges_Packet edges = createEdges(p.v1, p.v2);
            vector<char> se;
            serializeEdgesPacket(edges, se);
            sent = send(clientSocket, se.data(), se.size(), 0);
            if (sent < 0)
            {
                cerr << "Error sending data..." << endl;
                exit(1);
            }
            valrecv = recv(clientSocket, buffer, 1024, 0);
            if (valrecv < 0)
            {
                cerr << "Error receiving data..." << endl;
                exit(1);
            }
            else if (valrecv == 0)
            {
                cout << "Connection closed by server" << endl;
                close(clientSocket);
                exit(1);
            }
            cout << buffer << endl;

            continue;
        }
        else if (p.operation == "kosaraju" || p.operation == "newedge" || p.operation == "removeedge")
        {
            vector<char> sp;
            serializeOpPacket(p, sp);
            sent = send(clientSocket, sp.data(), sp.size(), 0);
            if (sent < 0)
            {
                cerr << "Error sending data..." << endl;
                exit(1);
            }
            valrecv = recv(clientSocket, buffer, 1024, 0);
            if (valrecv < 0)
            {
                cerr << "Error receiving data..." << endl;
                exit(1);
            }
            else if (valrecv == 0)
            {
                cout << "Connection closed by server" << endl;
                close(clientSocket);
                exit(1);
            }
            cout << buffer << endl;
        }
        else
        {
            cout << "Invalid command" << endl;
            continue;
        }
    }
    close(clientSocket);
    return 0;
}