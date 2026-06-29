#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
using namespace std;
bool is_executable(const std::filesystem::path& p) {
    std::error_code ec;
    auto perms = std::filesystem::status(p, ec).permissions();
    if (ec) return false; // file doesn't exist, no access, etc.
    using std::filesystem::perms;
    return (perms & (perms::owner_exec | perms::group_exec | perms::others_exec))
            != perms::none;
}
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
    vector<string> commands;
    string token;
    istringstream stream(command);
    commands = { "echo", "type", "exit" };
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
    else if (tokens[0] == "type"){
      if (find(commands.begin(), commands.end(), tokens[1]) != commands.end()) {
        cout << tokens[1] << " is a shell builtin" << endl;
      }
      else {
        const char* path = getenv("PATH");
        if (path != nullptr) {
          string pathStr(path);
          istringstream pathStream(pathStr);
          string dir;
          bool found = false;
          while (getline(pathStream, dir, ':')) {
            string fullPath = dir + "/" + tokens[1];
            if (ifstream(fullPath).good() && is_executable(fullPath)) {
              cout << tokens[1] << " is " << fullPath << endl;
              found = true;
              break;
            }
          }
          if (!found) {
            cout << tokens[1] << ": not found" << endl;
          }
        }
      }
    }
    else {
      cout << command << ": command not found" << endl;
    }
}
}
