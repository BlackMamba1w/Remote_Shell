#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "funcs.hpp"
using namespace std;
int main() {
  string command;
  string token;
  vector<string> args;
  vector<string> commands;
  commands = { "echo", "type", "exit" };
  // Flush after every std::cout / std:cerr
  // TODO: Uncomment the code below to pass the first stage
  while (true) {
    cout << unitbuf;
    cerr << unitbuf;
    cout << "$ ";
    getline(cin, command);
    istringstream stream(command);
    string current;
    vector<string> tokens;
    vector<string> shellArgs = parseShellArgs(command);
    for (char c : command) {
      if (c == ' ' || c == '\t') {
        if (!current.empty()) {
            tokens.push_back(current);
            current.clear();
        }
      }
      else {
          current += c;
      }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    if (command == "exit") {
      break;
    }
    else if (combineArgs(shellArgs).find(">") != string::npos){
      vector<string> execArgs = shellArgs;
      redirection(execArgs);
    }
    else if (tokens[0] == "echo") {
      echo(command, tokens);
    }
    else if (tokens[0] == "type"){
      if (find(commands.begin(), commands.end(), tokens[1]) != commands.end()) {
        cout << shellArgs[1] << " is a shell builtin" << endl;
      }
      else {
        execution(shellArgs);
      }
    }
    else {
      const char* path = getenv("PATH");
      if (path != nullptr) {
        string pathStr(path);
        istringstream pathStream(pathStr);
        string dir;
        bool found = false;
        while (getline(pathStream, dir, ':')) {
          string fullPath = dir + "/" + shellArgs[0];
          if (ifstream(fullPath).good() && is_executable(fullPath)) {
            found = true;
            pid_t pid = fork();
            if (pid == 0) { 
              vector<char*> argv;
              for (const auto& arg : shellArgs) {
                argv.push_back(const_cast<char*>(arg.c_str()));
              }
              argv.push_back(nullptr); 
              execvp(fullPath.c_str(), argv.data());
              cerr << "Error executing command: " << endl;
              _exit(1); 
            }
            else if (pid > 0) { 
              int status;
              waitpid(pid, &status, 0);
            }
            else {
              cerr << "Execvp failed: Try again" << endl;
            }
            break;
          }
        }
        if (!found) {
          cout << shellArgs[0] << ": command not found" << endl;
        }
      }
    }
  }
}