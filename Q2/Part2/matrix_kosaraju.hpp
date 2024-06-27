#ifndef KOSARAJU_HPP
#define KOSARAJU_HPP

#include <vector>
using namespace std;

bool dfs(int curr, int des, vector<vector<int>> &adj, vector<int> &vis);

bool isPath(int src, int des, vector<vector<int>> &adj);

vector<vector<int>> findSCC(int n, vector<vector<int>> &a);

// get edge list from adjacency matrix
vector<vector<int>> getEdgeList(vector<vector<int>> &adj);

#endif // KOSARAJU_HPP