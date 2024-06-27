#include "list_kosaraju.hpp"

// Convert adjacency matrix to edge list
list<list<int>> getEdgeList(list<list<int>> &adj)
{
    list<list<int>> edgeList;
    int i = 0;
    for (auto &row : adj)
    {
        int j = 0;
        for (auto &val : row)
        {
            if (val != 0)
            {
                list<int> edge = {i, j};
                edgeList.push_back(edge);
            }
            ++j;
        }
        ++i;
    }
    return edgeList;
}

bool dfs(int curr, int des, list<list<int>> &adj, list<int> &vis)
{
    if (curr == des)
        return true;
    auto vit = vis.begin();
    advance(vit, curr);
    *vit = 1;

    auto it = adj.begin();
    advance(it, curr); // Move iterator 'it' to the 'curr' position

    for (int x : *it)
    {
        auto vit = vis.begin();
        advance(vit, x);
        if (!*vit)
        {
            if (dfs(x, des, adj, vis))
                return true;
        }
    }

    return false;
}

bool isPath(int src, int des, list<list<int>> &adj)
{
    list<int> vis(adj.size(), 0);
    return dfs(src, des, adj, vis);
}

list<list<int>> findSCC(int n, list<list<int>> &adjMat)
{
    list<list<int>> a = getEdgeList(adjMat);
    list<list<int>> ans;
    list<int> is_scc(n + 1, 0);
    list<list<int>> adj(n + 1);

    for (auto &edge : a)
    {
        auto it = edge.begin();
        int u = *it++;
        int v = *it;
        auto adj_it = adj.begin();
        advance(adj_it, u);
        adj_it->push_back(v);
    }

    for (int i = 1; i <= n; i++)
    {
        auto it = is_scc.begin();
        advance(it, i);
        if (!*it)
        {
            list<int> scc;
            scc.push_back(i);
            for (int j = i + 1; j <= n; j++)
            {
                it = is_scc.begin();
                advance(it, j);
                if (!*it && isPath(i, j, adj) && isPath(j, i, adj))
                {
                    scc.push_back(j);
                    *it = 1;
                }
            }
            ans.push_back(scc);
        }
    }
    return ans;
}