#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <list>
#include <vector>
#include <exception>
#include <arpa/inet.h>
#include <cstring>
#include "packets.hpp"

#define PORT 9034

using namespace std;

bool isValidCommand(const string &data)
{
    // Check if the data contains exactly one comma
    size_t comma_pos = data.find(',');
    if (comma_pos == string::npos || comma_pos == 0 || comma_pos == data.size() - 1)
    {
        return false;
    }
    // Check if both parts are integers
    string part1 = data.substr(0, comma_pos);
    string part2 = data.substr(comma_pos + 1);
    for (char c : part1)
    {
        if (!isdigit(c))
            return false;
    }
    for (char c : part2)
    {
        if (!isdigit(c))
            return false;
    }
    return true;
}

op_Packet parseData(string data)
{
    // split thee data into operation and values
    // data: "newgraph n,m" -> operation: "newgraph", v1: n, v2: m

    op_Packet p;

    if (data.find("newgraph") != string::npos)
    {
        if (data.size() < 9)
        {
            p.operation = "invalid";
            p.v1 = -1;
            p.v2 = -1;
            return p;
        }
        p.operation = "newgraph";
        data = data.substr(9); // Remove "newgraph "

        if (isValidCommand(data))
        {
            try
            {
                size_t comma_pos = data.find(',');
                p.v1 = stoi(data.substr(0, comma_pos));
                p.v2 = stoi(data.substr(comma_pos + 1));
            }
            catch (const exception &e)
            {
                p.operation = "invalid";
                p.v1 = -1;
                p.v2 = -1;
            }
        }
        else
        {
            p.operation = "invalid";
            p.v1 = -1;
            p.v2 = -1;
        }
    }
    else if (data.find("kosaraju") != string::npos)
    {
        p.operation = "kosaraju";
        p.v1 = -1;
        p.v2 = -1;
    }
    else if (data.find("newedge") != string::npos)
    {
        if (data.size() < 8)
        {
            p.operation = "invalid";
            p.v1 = -1;
            p.v2 = -1;
            return p;
        }
        p.operation = "newedge";
        data = data.substr(8); // Remove "newedge "
        if (isValidCommand(data))
        {
            try
            {
                size_t comma_pos = data.find(',');
                p.v1 = stoi(data.substr(0, comma_pos));
                p.v2 = stoi(data.substr(comma_pos + 1));
            }
            catch (const exception &e)
            {
                p.operation = "invalid";
                p.v1 = -1;
                p.v2 = -1;
            }
        }
        else
        {
            p.operation = "invalid";
            p.v1 = -1;
            p.v2 = -1;
        }
    }
    else if (data.find("removeedge") == 0)
    {
        if (data.size() < 11)
        {
            p.operation = "invalid";
            p.v1 = -1;
            p.v2 = -1;
            return p;
        }
        p.operation = "removeedge";
        data = data.substr(11); // Remove "removeedge "

        if (isValidCommand(data))
        {
            try
            {
                size_t comma_pos = data.find(',');
                p.v1 = stoi(data.substr(0, comma_pos));
                p.v2 = stoi(data.substr(comma_pos + 1));
            }
            catch (const exception &e)
            {
                p.operation = "invalid";
                p.v1 = -1;
                p.v2 = -1;
            }
        }
        else
        {
            p.operation = "invalid";
            p.v1 = -1;
            p.v2 = -1;
        }
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

bool parceEdge(string &edge, int &u, int &v)
{
    size_t comma_pos = edge.find(',');
    if (comma_pos == string::npos || comma_pos == 0 || comma_pos == edge.size() - 1)
    {
        return false;
    }
    string part1 = edge.substr(0, comma_pos);
    string part2 = edge.substr(comma_pos + 1);
    for (char c : part1)
    {
        if (!isdigit(c))
            return false;
    }
    for (char c : part2)
    {
        if (!isdigit(c))
            return false;
    }
    u = stoi(part1);
    v = stoi(part2);
    return true;
}

edges_Packet createEdges(int n, int m)
{
    edges_Packet p;
    vector<list<int>> edges;

    for (int i = 0; i < m; i++)
    {
        int u, v;
        cout << "Enter edge " << i + 1 << ": ";
        string edge;
        cin >> edge;

        if (!parceEdge(edge, u, v)){
            cout << "Invalid edge" << endl;
            i--;
            continue;
        }

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

    // clear cin buffer
    cin.ignore(1024, '\n');

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

    cout << "Connected to server on port " << PORT << endl;

    int sent, valrecv;
    while (true)
    {
        cout << "      ---Graph Operations---" << endl;
        cout << "Enter one of the following commands: " << endl;
        cout << "-newgraph n,m" << endl;
        cout << "-kosaraju" << endl;
        cout << "-newedge u,v" << endl;
        cout << "-removeedge u,v" << endl;
        cout << "-exit\n"
             << endl;
        cout << "Enter command: ";

        string data;
        getline(cin, data);

        op_Packet p = parseData(data);

        if (p.operation == "invalid")
        {
            cout << "Invalid command" << endl;
        }
        else if (p.operation == "exit")
        {
            cout << "Connection terminated" << endl;
            close(clientSocket);
            exit(0);
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
            cout << "Sent command to server..." << endl;
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
            // clear buffer
            memset(buffer, 0, 1024);
        }
        else
        {
            cout << "Invalid command" << endl;
        }
    }
    close(clientSocket);
    return 0;
}
