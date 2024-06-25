#include <iostream>
#include <vector>
#include <list>
#include <string>
#include "vectorList_kosaraju.hpp"

using namespace std;

bool checkCommand(string command)
{
    if (command.find(",") == string::npos)
    {
        cout << "Invalid command. Please try again." << endl;
        return false;
    }
    else
    {
        int pos = command.find(",");
        // i need to check if before the comma and after the comma are numbers
        // if they are not numbers then the command is invalid
        // if they are numbers then I will convert them to integers
        // and then I will create the graph
        if (command.substr(0, pos).find_first_not_of("0123456789") != string::npos || command.substr(pos + 1).find_first_not_of("0123456789") != string::npos)
        {
            cout << "Invalid command. Please try again." << endl;
            return false;
        }
    }
    return true;
}

int main()
{
    vectorList_kosaraju *graph = nullptr;
    cout << "Welcome to the Kosaraju's Algorithm !!" << endl;
    cout << "to start using the program, you must initialize the graph. To do that start by writing NewGraph n,m" << endl;

    string command;

    while (true)
    {
        cin >> command;
        transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "newgraph")
        {
            cin >> command;

            if (checkCommand(command))
            {
                int n, m;
                int pos = command.find(",");
                n = stoi(command.substr(0, pos));
                m = stoi(command.substr(pos + 1));
                graph = new vectorList_kosaraju(n, m);
            }
        }
        else if (command == "kosaraju")
        {
            cout << "Calculating the Strongly Connected Components using Kosaraju's Algorithm..." << endl;
            graph->printSCC();
        }
        else if (command == "printgraph")
        {
            graph->printGraph();
        }
        else if (command == "newedge")
        {
            cin >> command;
            if (checkCommand(command))
            {
                int u, v;
                int pos = command.find(",");
                u = stoi(command.substr(0, pos));
                v = stoi(command.substr(pos + 1));
                graph->addEdge(u, v);
            }
        }
        else if (command == "removeedge")
        {
            cin >> command;
            if (checkCommand(command))
            {
                int u, v;
                int pos = command.find(",");
                u = stoi(command.substr(0, pos));
                v = stoi(command.substr(pos + 1));
                graph->removeEdge(u, v);
            }
        }
        else if (command == "exit")
        {
            break;
        }
        else
        {
            cout << "Invalid command. Please try again." << endl;
        }

        cout << "--------------------------------------------" << endl;
        cout << "the commands are: " << endl;
        cout << "NewGraph n,m" << endl;
        cout << "Kosaraju" << endl;
        cout << "PrintGraph" << endl;
        cout << "NewEdge u,v" << endl;
        cout << "RemoveEdge u,v" << endl;
        cout << "Exit" << endl;
        cout << "Please enter a command: ";
    }

    return 0;
}