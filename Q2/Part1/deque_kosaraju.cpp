#include "deque_kosaraju.hpp"

// Convert adjacency matrix to edge list
deque<deque<int>> getEdgeList(deque<deque<int>> &adj)
{
    deque<deque<int>> edgeList;
    for (size_t i = 1; i < adj.size(); i++)
    {
        for (size_t j = 1; j < adj[i].size(); j++)
        {
            if (adj[i][j] != 0)
            {
                deque<int> edge = {static_cast<int>(i), static_cast<int>(j)};
                edgeList.push_back(edge);
            }
        }
    }
    return edgeList;
}

bool dfs(int curr, int des, deque<deque<int>> &adj, deque<int> &vis)
{
    if (curr == des)
        return true;
    vis[curr] = 1;
    for (size_t i = 0; i < adj[curr].size(); i++)
    {
        int x = adj[curr][i];
        if (!vis[x])
        {
            if (dfs(x, des, adj, vis))
                return true;
        }
    }
    return false;
}

bool isPath(int src, int des, deque<deque<int>> &adj)
{
    deque<int> vis(adj.size() + 1, 0);
    return dfs(src, des, adj, vis);
}

deque<deque<int>> findSCC(int n, deque<deque<int>> &adjMat)
{
    deque<deque<int>> a = getEdgeList(adjMat);
    deque<deque<int>> ans;
    deque<int> is_scc(n + 1, 0);
    deque<deque<int>> adj(n + 1);

    for (size_t i = 0; i < a.size(); i++)
    {
        adj[a[i][0]].push_back(a[i][1]);
    }

    for (int i = 1; i <= n; i++)
    {
        if (!is_scc[i])
        {
            deque<int> scc;
            scc.push_back(i);
            for (int j = i + 1; j <= n; j++)
            {
                if (!is_scc[j] && isPath(i, j, adj) && isPath(j, i, adj))
                {
                    is_scc[j] = 1;
                    scc.push_back(j);
                }
            }
            ans.push_back(scc);
        }
    }
    return ans;
}
