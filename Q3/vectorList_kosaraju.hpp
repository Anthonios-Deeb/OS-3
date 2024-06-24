#ifndef VECTORLIST_KOSARAJU_HPP
#define VECTORLIST_KOSARAJU_HPP
#include <vector>
#include <list>
#include <iostream>
using namespace std;

class vectorList_kosaraju
{
public:

vectorList_kosaraju() = default;
vectorList_kosaraju(vector<list<int>> &a);



vector<list<int>> findSCC();
// get edge list from adjacency matrix
vector<list<int>> getEdgeList();

//prints
void printGraph();
void printEdgeList();
void printSCC();

void addEdge(int u, int v);
void removeEdge(int u, int v);


private:
vector<list<int>> graph;
vector<list<int>> tgraph;
vector<list<int>> scc;
vector<list<int>> edgeList;

bool dfs(int curr, int des, vector<list<int>> &adj, vector<int> &vis);
bool isPath(int src, int des, vector<list<int>> &adj);
};

#endif // VECTORLIST_KOSARAJU_HPP