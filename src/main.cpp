#include <iostream>
#include <string>
using namespace std;
int main() {
  // Flush after every std::cout / std:cerr
  cout << unitbuf;
  cerr << unitbuf;

  // TODO: Uncomment the code below to pass the first stage
while (true) {
    cout << "$ ";
    string command;
    getline(cin, command);
    string[] args = split(command, ' ');
    if (command == "exit") {
      break;
    }
    else if (args[0] == "echo") {
      for (int i = 1; i < args.size(); i++) {
        cout << args[i] << " ";
      }
      cout << endl;
    }
    else {
      cout << command << ": command not found" << endl;
    }
}
}
