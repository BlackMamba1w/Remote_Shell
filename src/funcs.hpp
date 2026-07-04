#pragma once

#include <filesystem>
#include <string>
#include <vector>
std::vector<std::string> parseShellArgs(const std::string& cmd);
std::vector<int> ocurrences(std::string str, char ch);
std::string trim(std::string s);
bool is_executable(const std::filesystem::path& p);
bool hasQuotes(const std::string& str);
void execution(const std::vector<std::string>& tokens);
std::string removeCharacters(const std::string& str, char ch);
std::string combineArgs (const std::vector<std::string>& args);
std::string readFile(const std::vector<std::string>& args);
void writeFile(const std::string& res, const std::vector<std::string>& args);
void redirection(const std::vector<std::string>& execArgs);
void echo(const std::string& command, const std::vector<std::string>& tokens);