#include <iostream>

using namespace std;




int main() {
    int n, m;
    cout << "Please enter the number of nodes and the number of edges: " << endl;
    cin >> n >> m;

    //check if the input is valid and if the input is a number

    while (n < 1 || m < 1){
        if (cin.fail()){
            cin.clear();
            cin.ignore();
        }
        cout << "Invalid input. Please enter positive integers." << endl;
        cin >> n >> m;
    }

    return 0;
}
