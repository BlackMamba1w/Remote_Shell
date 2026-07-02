#include "funcs.hpp"

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
void execution( const vector<string>& tokens) {
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
vector<string> parseShellArgs(const string& cmd, char delimiter) {
    vector<string> args;
    string current;
    bool inQuotes = false;
    bool tokenStarted = false; 
    for (size_t i = 0; i < cmd.size(); ++i) {
        char c = cmd[i];
        if (c == delimiter && cmd[i-1] != '\\') { // Check for unescaped quote
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
    for (auto& arg : args) {
        for (size_t i = 0; i < arg.size(); ++i) {
            if (arg[i] == '\\' && arg[i-1] != '\\') { // Check for unescaped backslash
                arg.erase(i, 1); // Remove the backslash
            }
        }
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
    string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if ((s[i] != ' ' && s[i] != '\t' && s[i] != '\\') || s[i-1] == '\\') {
            result += s[i];
        }
    }
    return result;
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
    return count > 1;
}