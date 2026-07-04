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
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#define CHUNK_SIZE 1024
using namespace std;
vector<string> commands = { "echo", "type", "exit" };
string readFile(const vector<string>& args){
    size_t redirectPos = string::npos;
    string result;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == ">" || args[i] == "1>") {
            redirectPos = i;
            break;
        }
    }
    if (filesystem::exists(args[redirectPos - 1])) {
        ifstream inFile(args[redirectPos - 1]);
        string line;
        while (getline(inFile, line)) {
            result += line + '\n';
        }
        inFile.close();
    }
    return result;
}
void echo(const string& command, const vector<string>& tokens){
    if (hasQuotes(command)) {
        vector<string> args = parseShellArgs(command);
        cout << combineArgs(args) << endl;
    }
    else {
        for (size_t i = 1; i < tokens.size(); ++i) {
            cout << trim(tokens[i]) << " ";
        }
        cout << endl;
    }
}
void writeFile(const string& res, const vector<string>& args){
    size_t redirectPos = string::npos;
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == ">" || args[i] == "1>") {
            redirectPos = i;
            break;
        }
    }
    if (filesystem::exists(args[redirectPos + 1])){
        ofstream outFile(args[redirectPos + 1]);
        outFile << res;
        outFile.close();
    }
    else{
        ofstream file(args[redirectPos + 1]);
        file << res;
        file.close();
    }
}
void redirection(const vector<string>& execArgs) {
    if (execArgs[0] == "echo"){
        writeFile(readFile(execArgs), execArgs);
    }
    if (execArgs[0] == "type"){
        string res;
        if (find(commands.begin(), commands.end(), execArgs[1]) != commands.end()) {
            res += execArgs[1] + " is a shell builtin";
            writeFile(res, execArgs);
        }
        else {
            const char* path = getenv("PATH");
            if (path != nullptr) {
                string pathStr(path);
                istringstream pathStream(pathStr);
                string dir;
                bool found = false;
                while (getline(pathStream, dir, ':')) {
                    string fullPath = dir + "/" + execArgs[1];
                    if (ifstream(fullPath).good() && is_executable(fullPath)) {
                    res += execArgs[1] + " is " + fullPath;
                    found = true;
                    break;
                    }
                }
            }
            writeFile(res, execArgs);
        }
    }
    else {
        size_t redirectPos = string::npos;
        for (size_t i = 0; i < execArgs.size(); ++i) {
            if (execArgs[i] == ">" || execArgs[i] == "1>") {
                redirectPos = i;
                break;
            }
        }
        if (redirectPos == string::npos || redirectPos + 1 >= execArgs.size()) {
            cerr << "Redirection syntax error.\n";
            return;
        }
        vector<char*> argv;
        for (size_t i = 0; i < redirectPos; ++i) {
            argv.push_back(const_cast<char*>(execArgs[i].c_str()));
        }
        argv.push_back(nullptr);
        const char* outFile = execArgs[redirectPos + 1].c_str();
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        }
        if (pid == 0) { 
            int fd = open(outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                exit(1);
            }
            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("dup2");
                exit(1);
            }
            close(fd);
            execvp(argv[0], argv.data());
            cerr << "execvp failed" << endl;
            exit(1);
        }
        else {
            waitpid(pid, nullptr, 0);
        }
    }
}

void execution(const vector<string>& tokens) {
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
vector<string> parseShellArgs(const string& cmd) {
    vector<string> args;
    string current;
    bool inSingleQuotes = false;
    bool inDoubleQuotes = false;
    bool tokenStarted = false;
    for (size_t i = 0; i < cmd.size(); ++i) {
        char c = cmd[i];
        if (inSingleQuotes) {
            if (c == '\'') {
                inSingleQuotes = false;
            } else {
                current += c;
                tokenStarted = true;
            }
        }
        else if (inDoubleQuotes) {
            if (c == '"') {
                inDoubleQuotes = false;
            }
            else if (c == '\\' && i + 1 < cmd.size()) {
                char next = cmd[i + 1];
                if (next == '"' || next == '\\' || next == '\'' ) {
                    current += next;
                    ++i;
                } else {
                    current += c;
                }
                tokenStarted = true;
            }
            else {
                current += c;
                tokenStarted = true;
            }
        }
        else {
            if (c == '\'') {
                inSingleQuotes = true;
                tokenStarted = true; 
            }
            else if (c == '"') {
                inDoubleQuotes = true;
                tokenStarted = true;
            }
            else if (c == '\\' && i + 1 < cmd.size()) {
                current += cmd[++i];
                tokenStarted = true;
            }
            else if (c == ' ' || c == '\t') {
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
    }
    if (tokenStarted) {
        args.push_back(current);
    }
    return args;
}
string combineArgs (const vector<string>& args) {
    string combined;
    for (int i = 1; i < args.size(); ++i) {
        combined += args[i] + " ";
    }
    return combined.substr(0, combined.size() - 1);
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
    string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if ((s[i] != ' ' && s[i] != '\t' && s[i] != '\\') || s[i-1] == '\\') {
            result += s[i];
        }
    }
    return result;
}
bool is_executable(const filesystem::path& p) {
    error_code ec;
    auto perms = filesystem::status(p, ec).permissions();
    if (ec) return false; // file doesn't exist, no access, etc.
    using filesystem::perms;
    return (perms & (perms::owner_exec | perms::group_exec | perms::others_exec))
            != perms::none;
}
bool hasQuotes(const string& str) {
    int count = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (( str[i] == '\'' && str[i-1] != '\\') || (str[i] == '\"' && str[i-1] != '\\' )) { // single or double quote character
            count++;
        }
    }
    return count > 1;
}
string removeCharacters(const string& str, char ch) {
    string result;
    for (char c : str) {
        if (c != ch) {
            result += c;
        }
    }
    return result;
}
