#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <vector>
#include <list>
#include <arpa/inet.h>
#include <cstring>
#include <string>
using namespace std;

struct op_Packet
{
    string operation; // newgraph, kosaraju, newedge, removeedge, exit
    int v1;             // n or u or empty(for kosaraju)
    int v2;             // m or v or empty(for kosaraju)
};

struct edges_Packet
{
    int n;
    int m;
    vector<list<int>> edges;
};

void serializeOpPacket(const op_Packet& packet, std::vector<char>& buffer);
void deserializeOpPacket(const char* buffer, op_Packet& packet);
void serializeEdgesPacket(const edges_Packet &packet, vector<char> &buffer);
void deserializeEdgesPacket(const char *buffer, edges_Packet &packet);

#endif // PACKETS_HPP
