#ifndef VECTORLIST_KOSARAJU_HPP
#define VECTORLIST_KOSARAJU_HPP
#include <vector>
#include <list>
using namespace std;

bool dfs(int curr, int des, vector<list<int>> &adj, vector<int> &vis);

bool isPath(int src, int des, vector<list<int>> &adj);

vector<list<int>> findSCC(int n, vector<list<int>> &a);

// get edge list from adjacency matrix
vector<list<int>> getEdgeList(vector<list<int>> &adj);

#endif // VECTORLIST_KOSARAJU_HPP