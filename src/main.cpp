#include <iostream>
#include <string>
#include <vector>
#include <sstream>
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
    vector<string> args;
    vector<string> tokens;
    string token;
    istringstream stream(command);
    while (getline(stream, token, ' ')) {
        tokens.push_back(token);
    }
    if (command == "exit") {
      break;
    }
    else if (tokens[0] == "echo") {
      for (int i = 1; i < tokens.size(); i++) {
        cout << tokens[i] << " ";
      }
      cout << endl;
    }
    else {
      cout << command << ": command not found" << endl;
    }
}
}
