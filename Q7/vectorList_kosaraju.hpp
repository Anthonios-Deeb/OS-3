#ifndef VECTORLIST_KOSARAJU_HPP
#define VECTORLIST_KOSARAJU_HPP
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>
using namespace std;

class vectorList_kosaraju
{
public:
    vectorList_kosaraju(int n, int m);
    vectorList_kosaraju(int n, int m, vector<list<int>> &adj);

    vector<list<int>> findSCC();     // Kosaraju's Algorithm
    vector<list<int>> getEdgeList(); // Get the edge list

    // prints
    void printGraph();
    void printEdgeList();
    void printSCC();

    int addEdge(int u, int v);
    int removeEdge(int u, int v);

private:
    vector<list<int>> graph;
    vector<list<int>> scc;
    vector<list<int>> edgeList;

    void init(int n, int m);

    bool dfs(int curr, int des, vector<list<int>> &adj, vector<int> &vis);
    bool isPath(int src, int des, vector<list<int>> &adj);
};

#endif // VECTORLIST_KOSARAJU_HPP