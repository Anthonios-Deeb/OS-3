#ifndef LISTKOSARAJU_HPP
#define LISTKOSARAJU_HPP

#include <list>
using namespace std;

bool dfs(int curr, int des, list<list<int>> &adj, list<int> &vis);

bool isPath(int src, int des, list<list<int>> &adj);

list<list<int>> findSCC(int n, list<list<int>> &a);

// get edge list from adjacency matrix
list<list<int>> getEdgeList(list<list<int>> &adj);

#endif // LISTKOSARAJU_HPP
