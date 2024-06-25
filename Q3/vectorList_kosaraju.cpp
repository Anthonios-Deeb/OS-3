#include "vectorList_kosaraju.hpp"

vectorList_kosaraju::vectorList_kosaraju(int n,int m)
{
    cout << "Creating a new graph..." << endl;
    init(n, m);
    this->edgeList = getEdgeList();
    this->scc = findSCC();
}

void vectorList_kosaraju::init(int n , int m)
{
    // Resize the graph
    graph.resize(n + 1);

    // Fill the adjacency list
    for (int i = 0; i < m; i++)
    {
        int u, v;
        cout << "Please enter the edge " << i + 1 << ". <u v>: " << endl;
        cin >> u >> v;

        // check if the input is valid and if the input is a number
        while (u <= 0 || v <= 0 || u > n || v > n || u == v)
        {
            if (cin.fail())
            {
                cin.clear();
                cin.ignore();
            }
            cout << "Invalid input. Please enter positive integers." << endl;
            cin >> u >> v;
        }

        // check if the edge is already present
        auto it = find(graph[u].begin(), graph[u].end(), v);
        if (it == graph[u].end())
        {
            graph[u].push_back(v);
        }
        else
        {
            cout << "The edge is already present." << endl;
            continue;
        }
    }
}

vector<list<int>> vectorList_kosaraju::getEdgeList()
{
    vector<list<int>> ans;
    for (size_t i = 0; i < this->graph.size(); i++)
    {
        for (auto x : this->graph[i])
        {
            list<int> temp;
            temp.push_back(i);
            temp.push_back(x);
            ans.push_back(temp);
        }
    }
    return ans;
}


bool vectorList_kosaraju::dfs(int curr, int des, vector<list<int>> &adj, vector<int> &vis)
{
    // If curr node is destination return true
    if (curr == des)
    {
        return true;
    }
    vis[curr] = 1;
    for (auto x : adj[curr])
    {
        if (!vis[x])
        {
            if (dfs(x, des, adj, vis))
            {
                return true;
            }
        }
    }
    return false;
}

bool vectorList_kosaraju::isPath(int src, int des, vector<list<int>> &adj)
{
    vector<int> vis(adj.size() + 1, 0);
    return dfs(src, des, adj, vis);
}

vector<list<int>> vectorList_kosaraju::findSCC()
{
    // Stores all the strongly connected components.
    vector<list<int>> ans;

    // Stores whether a vertex is a part of any Strongly
    // Connected Component
    vector<int> is_scc(graph.size(), 0);

    vector<list<int>> adj(graph.size());

    for (size_t i = 0; i < edgeList.size(); i++)
    {
        adj[edgeList[i].front()].push_back(edgeList[i].back());
    }

    // Traversing all the vertices
    for (size_t i = 1; i < graph.size(); i++)
    {

        if (!is_scc[i])
        {
            // If a vertex i is not a part of any SCC
            // insert it into a new SCC list and check
            // for other vertices whether they can be
            // thr part of thidl ist.
            list<int> scc;
            scc.push_back(i);

            for (size_t j = i + 1; j < graph.size(); j++)
            {
                // If there is a path from vertex i to
                // vertex j and vice versa put vertex j
                // into the current SCC list.
                if (!is_scc[j] && isPath(i, j, adj) && isPath(j, i, adj))
                {
                    is_scc[j] = 1;
                    scc.push_back(j);
                }
            }

            // Insert the SCC containing vertex i into
            // the final list.
            ans.push_back(scc);
        }
    }
    return ans;
}

void vectorList_kosaraju::printGraph()
{
    for (size_t i = 1; i < graph.size(); i++)
    {
        cout << "Vertex " << i << ": ";
        for (auto it = graph[i].begin(); it != graph[i].end(); it++)
        {
            cout << *it << " ";
        }
        cout << endl;
    }
}

void vectorList_kosaraju::printEdgeList()
{
    for (size_t i = 0; i < edgeList.size(); i++)
    {
        cout << "Edge " << i + 1 << ": ";
        for (auto it = edgeList[i].begin(); it != edgeList[i].end(); it++)
        {
            cout << *it << " ";
        }
        cout << endl;
    }
}

void vectorList_kosaraju::printSCC()
{
    cout << "The strongly connected components are: " << endl;
    for (size_t i = 0; i < scc.size(); i++)
    {
        cout << "SCC " << i + 1 << ": ";
        for (auto it = scc[i].begin(); it != scc[i].end(); it++)
        {
            cout << *it << " ";
        }
        cout << endl;
    }
}

void vectorList_kosaraju::addEdge(int u, int v)
{
    if(u <= 0 || v <= 0 || u > this->graph.size() - 1 || v > this->graph.size() - 1 || u == v)
    {
        cout << "Invalid input. Out of bounds." << endl;
        return;
    }

    auto it = find(graph[u].begin(), graph[u].end(), v);
    if (it == graph[u].end())
    {
        graph[u].push_back(v);
        this->edgeList = getEdgeList();
        this->scc = this->findSCC();
    }
    else
    {
        cout << "The edge is already present." << endl;
    }
}

void vectorList_kosaraju::removeEdge(int u, int v)
{
    if(u <= 0 || v <= 0 || u > this->graph.size() - 1 || v > this->graph.size() - 1 || u == v)
    {
        cout << "Invalid input. Out of bounds." << endl;
        return;
    }

    auto it = find(graph[u].begin(), graph[u].end(), v);
    if (it != graph[u].end())
    {
        graph[u].remove(v);
        this->edgeList = getEdgeList();
        this->scc = this->findSCC();
    }
    else
    {
        cout << "The edge is not present." << endl;
    }
}