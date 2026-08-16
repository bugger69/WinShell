#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>

namespace fs = std::filesystem;

#define EXEC_PERMISSIONS (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)

bool execute_permission(const fs::path exec_path);
const char* find_path_var();