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
string combineArgs (const vector<string>& args){
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
