#include <iostream>
#include <list>
#include "list_kosaraju.hpp"
#include <limits>

using namespace std;

// Print the adjacency matrix
void printAdjacencyList(list<list<int>> adjMatrix) {
    cout << "The adjacency matrix is: " << endl;
    int i = 1;
    for (auto& row : adjMatrix) {
        if (i == 1) {
            ++i;
            continue;
        }
        int j = 1;
        for (auto& val : row) {
            if (j != 1) {
                cout << val << " ";
            }
            ++j;
        }
        cout << endl;
        ++i;
    }
}

int main() {
    int n, m;
    cout << "Please enter the number of nodes and the number of edges: " << endl;
    cin >> n >> m;

    // Check if the input is valid and if the input is a number
    while (n < 1 || m < 1 || m > n * (n - 1)) {
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout << "Invalid input. Please enter positive integers." << endl;
        cin >> n >> m;
    }

    // Create an adjacency matrix using list
    list<list<int>> adjMatrix(n + 1, list<int>(n + 1, 0));

    // Fill the adjacency matrix
    for (int i = 0; i < m; i++) {
        int u, v;
        cout << "Please enter the edge " << i + 1 << ". <u v>: " << endl;
        cin >> u >> v;

        // Check if the input is valid and if the input is a number
        while (u <= 0 || v <= 0 || u > n || v > n || u == v) {
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            cout << "Invalid input. Please enter positive integers." << endl;
            cin >> u >> v;
        }

        // Check if the edge is already present
        auto row_it = adjMatrix.begin();
        advance(row_it, u);
        auto col_it = row_it->begin();
        advance(col_it, v);

        if (*col_it == 1){
            cout << "The edge is already present. Please enter a different edge." << endl;
            i--;
            continue;
        }

        *col_it = 1;
    }

    printAdjacencyList(adjMatrix);

    list<list<int>> edges = getEdgeList(adjMatrix);

    int edgeNum = 1;
    for (auto& edge : edges) {
        auto it = edge.begin();
        cout << "Edge " << edgeNum++ << ": " << *it++ << " " << *it << endl;
    }

    // Find the strongly connected components
    list<list<int>> scc = findSCC(n, adjMatrix);

    cout << "The strongly connected components are: " << endl;
    int compNum = 1;
    for (auto& component : scc) {
        cout << "Component " << compNum++ << ": ";
        for (auto& node : component) {
            cout << node << " ";
        }
        cout << endl;
    }
    return 0;
}