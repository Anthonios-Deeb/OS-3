#ifndef DEQUEKOSARAJU_HPP
#define DEQUEKOSARAJU_HPP

#include <deque>
using namespace std;

bool dfs(int curr, int des, deque<deque<int>> &adj, deque<int> &vis);

bool isPath(int src, int des, deque<deque<int>> &adj);

deque<deque<int>> findSCC(int n, deque<deque<int>> &a);

// get edge list from adjacency matrix
deque<deque<int>> getEdgeList(deque<deque<int>> &adj);

#endif // DEQUEKOSARAJU_HPP
