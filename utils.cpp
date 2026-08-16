#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include "utils.hpp"

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