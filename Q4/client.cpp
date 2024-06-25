#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9034

using namespace std;

int main()
{
    int clientSocket;
    sockaddr_in serverAddr;
    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Error establishing socket..." << endl;
        exit(1);
    }
    cout << "Client Socket created..." << endl;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Error connecting to server..." << endl;
        exit(1);
    }

    cout << "Connected to server..." << endl;

    cout << "Enter one of the following commands: " << endl;
    cout << "NewGraph n,m" << endl;
    cout << "Kosaraju" << endl;
    cout << "NewEdge u,v" << endl;
    cout << "RemoveEdge u,v" << endl;
    cout << "Exit" << endl;

    while (true)
    {
        string data;
        getline(cin, data);

        send(clientSocket, data.c_str(), data.size() + 1, 0);

        if (data == "Exit")
        {
            close(clientSocket);
            cout << "Connection terminated..." << endl;
            exit(1);
        }

        if (recv(clientSocket, buffer, 1024, 0) < 0)
        {
            cerr << "Error in receiving data..." << endl;
        }
        else
        {
            cout << "Server: ";
            cout << buffer << endl;
        }
    }
    close(clientSocket);
    return 0;
}