#include <iostream>
#include <vector>
#include <list>
#include "vectorList_kosaraju.hpp"

using namespace std;


int main()
{
    int n, m;
    cout << "Please enter the number of nodes and the number of edges: " << endl;
    cin >> n >> m;

    // check if the input is valid and if the input is a number
    while (n < 1 || m < 1 || m > n * (n - 1))
    {
        if (cin.fail())
        {
            cin.clear();
            cin.ignore();
        }
        cout << "Invalid input. Please enter positive integers." << endl;
        cin >> n >> m;
    }

    // Create an adjacency list
    vector<list<int>> adjMatrix(n + 1);

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
        auto it = find(adjMatrix[u].begin(), adjMatrix[u].end(), v);
        if (it == adjMatrix[u].end())
        {
            adjMatrix[u].push_back(v);
        }
        else
        {
            cout << "The edge is already present." << endl;
            continue;
        }
    }

    vectorList_kosaraju vlk = vectorList_kosaraju(adjMatrix);
    
    vlk.printEdgeList();
    
    vlk.printSCC();

    return 0;
}