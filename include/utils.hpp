#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>

namespace fs = std::filesystem;

#define DELIMITERS " \t\r\n\a"
#define DELIMITER " \r\n"
#define DOUBLEQUOTESPCHARS {'"', '\\'}
#define EXEC_PERMISSIONS (fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec)

/* Path helpers */
bool execute_permission(const fs::path exec_path);
const char* find_path_var();

/* Escape Char helpers */
bool isEscape(const std::string &line, int i);
bool isCurrEscape(const std::string &line, int i);
bool isDoubleQuoteSp(const std::string &line, int i);