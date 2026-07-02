#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> parseShellArgs(const std::string& cmd, char delimiter);
std::vector<int> ocurrences(std::string str, char ch);
std::string trim(std::string s);
bool is_executable(const std::filesystem::path& p);
bool hasQuotes(const std::string& str);
void execution(const std::vector<std::string>& tokens);