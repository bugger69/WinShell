#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include "utils.hpp"

bool has_space(const std::string& cmd) {
    for(auto it : cmd) {
        if(it == ' ') return true;
    }
    return false;
}

bool endsWith(const std::string& mainStr, const std::string& suffix) {
    if (mainStr.length() < suffix.length()) return false;
    return mainStr.rfind(suffix) == (mainStr.length() - suffix.length());
}

const char* find_path_var() {
    const char* path_env = std::getenv("PATH");
    if(!path_env) {
        std::cout << "WinShell: unable to find path." << std::endl;
    }
    return path_env;
}

bool execute_permission(const fs::path exec_path) {
    if(!fs::exists(exec_path)) {
        return false;
    }
    fs::perms p = fs::status(exec_path).permissions();
    return ((p & EXEC_PERMISSIONS) != fs::perms::none);
}

bool isEscape(const std::string &line, int i) {
    if(i > 0 && line[i - 1] == '\\') return true;
    return false;
}

bool isCurrEscape(const std::string &line, int i) {
    if(i > 0 && line[i - 1] != '\\' && line[i] == '\\') return true;
    return false;
}

bool isDoubleQuoteSp(const std::string &line, int i) {
    for(auto it : DOUBLEQUOTESPCHARS) {
        if(i < line.size() - 1 && line[i] == '\\' && line[i + 1] == it) {
            return true;
        }
    }
    return false;
}