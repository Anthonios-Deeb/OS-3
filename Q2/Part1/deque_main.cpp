#include <iostream>
#include <deque>
#include "deque_kosaraju.hpp"
#include <limits>

using namespace std;

// print the adjacency matrix
void printAdjacencyList(deque<deque<int>> adjMatrix)
{
    cout << "The adjacency matrix is: " << endl;
    for (size_t i = 1; i < adjMatrix.size(); i++)
    {
        for (size_t j = 1; j < adjMatrix[i].size(); j++)
        {
            cout << adjMatrix[i][j] << " ";
        }
        cout << endl;
    }
}

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
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout << "Invalid input. Please enter positive integers." << endl;
        cin >> n >> m;
    }

    // Create an adjacency matrix using deque
    deque<deque<int>> adjMatrix(n + 1, deque<int>(n + 1, 0));

    // Fill the adjacency matrix
    for (int i = 0; i < m; i++)
    {
        int u, v;
        cout << "Please enter the edge " << i + 1 << ". <u v>: " << endl;
        cin >> u >> v;

        // Check if the input is valid and if the input is a number
        while (u <= 0 || v <= 0 || u > n || v > n || u == v)
        {
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cout << "Invalid input. Please enter positive integers." << endl;
            cin >> u >> v;
        }

        // Check if the edge is already present
        if (adjMatrix[u][v] == 1)
        {
            cout << "The edge is already present. Please enter a different edge." << endl;
            i--;
            continue;
        }

        adjMatrix[u][v] = 1;
    }

    // printAdjacencyList(adjMatrix);

    // Assuming you have implemented getEdgeList and findSCC in kosaraju.hpp
    deque<deque<int>> edges = getEdgeList(adjMatrix);

    // for (size_t i = 0; i < edges.size(); i++) {
    //     cout << "Edge " << i + 1 << ": " << edges[i][0] << " " << edges[i][1] << endl;
    // }

    // Find the strongly connected components
    deque<deque<int>> scc = findSCC(n, adjMatrix);

    cout << "The strongly connected components are: " << endl;
    for (size_t i = 0; i < scc.size(); i++)
    {
        cout << "Component " << i + 1 << ": ";
        for (size_t j = 0; j < scc[i].size(); j++)
        {
            cout << scc[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
}
