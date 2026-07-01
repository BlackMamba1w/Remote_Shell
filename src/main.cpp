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
using namespace std;
vector<string> parseShellArgs(const string& cmd, char delimiter) {
    vector<string> args;
    string current;
    bool inQuotes = false;
    bool tokenStarted = false; 
    for (size_t i = 0; i < cmd.size(); ++i) {
        char c = cmd[i];
        if (c == delimiter) {
            inQuotes = !inQuotes;
            tokenStarted = true; 
        }
        else if ((c == ' ' || c == '\t') && !inQuotes) {
            if (tokenStarted) {
                args.push_back(current);
                current.clear();
                tokenStarted = false;
            }
        }
        else {
            current += c;
            tokenStarted = true;
        }
    }
    if (tokenStarted) {
        args.push_back(current);
    }
    return args;
}
vector<int> ocurrences(string str, char ch) {
    vector<int> positions;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == ch) {
            positions.push_back(i);
        }
    }
    return positions;
}
string trim(string s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");

    if (start == string::npos) return ""; // all whitespace or empty
    return s.substr(start, end - start + 1);
}
bool is_executable(const std::filesystem::path& p) {
    std::error_code ec;
    auto perms = std::filesystem::status(p, ec).permissions();
    if (ec) return false; // file doesn't exist, no access, etc.
    using std::filesystem::perms;
    return (perms & (perms::owner_exec | perms::group_exec | perms::others_exec))
            != perms::none;
}
bool hasQuotes(const string& str) {
    int count = 0;
    for (char ch : str) {
        if (ch == '\'' || ch == '\"') { // single or double quote character
            count++;
        }
    }
    return count > 0;
}
int main() {
    // Flush after every std::cout / std:cerr
    // TODO: Uncomment the code below to pass the first stage
    while (true) {
        cout << unitbuf;
        cerr << unitbuf;
        string command;
        string token;
        vector<string> args;
        vector<string> tokens;
        vector<string> commands;
        vector<string> tokens2;
        string cmd1;
        commands = { "echo", "type", "exit" };
        cout << "$ ";
        getline(cin, command);
        istringstream stream(command);
        string current;
        for (char c : command) {
            if (c == ' ' || c == '\t') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            tokens.push_back(current);
        }
        if (command == "exit") {
          break;
        }
        else if (tokens[0] == "echo") {
          if (hasQuotes(command)) {
              char quoteChar = (command.find('"') != string::npos) ? '"' : '\'';
              vector<string> args = parseShellArgs(command, quoteChar);
              for (size_t i = 1; i < args.size(); ++i) {
                  cout << args[i];
                  if (i + 1 < args.size()) cout << " ";
              }
              cout << endl;
          }
          else {
              for (size_t i = 1; i < tokens.size(); ++i) {
                  cout << trim(tokens[i]) << " ";
              }
              cout << endl;
    }
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
          const char* path = getenv("PATH");
            if (path != nullptr) {
              string pathStr(path);
              istringstream pathStream(pathStr);
              string dir;
              bool found = false;
              while (getline(pathStream, dir, ':')) {
                string fullPath = dir + "/" + tokens[0];
                if (ifstream(fullPath).good() && is_executable(fullPath)) {
                  found = true;
                  pid_t pid = fork();
                  if (pid == 0) { // Child process
                    vector<string> execArgs;
                    if (command.find('\"') != string::npos){
                      execArgs = parseShellArgs(command,'\"');
                    }
                    else {
                      execArgs = parseShellArgs(command,'\'');
                    }
                    vector<char*> argv;
                    for (const auto& arg : execArgs) {
                      argv.push_back(const_cast<char*>(arg.c_str()));
                    }
                    argv.push_back(nullptr); // Null-terminate the array
                    execvp(fullPath.c_str(), argv.data());
                    cerr << "Error executing command: " << endl;
                    _exit(1); // Exit child process on error
                }
                  else if (pid > 0) { // Parent process
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
                cout << tokens[0] << ": command not found" << endl;
            }
        }
    }
  }
}