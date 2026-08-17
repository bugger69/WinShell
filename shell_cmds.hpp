#include <iostream>
#include <string>
#include <vector>
#include <map>

// Type Headers
#define SHELL_CMD_TYPE_UNKNOWN -1
#define SHELL_CMD_TYPE_BUILTIN 0
#define SHELL_CMD_TYPE_EXEC 1

// Default executables array
#define EXEC_BIN_PATH ""

int shell_chdir(std::vector<std::string> &args);
int shell_echo(std::vector<std::string> &args);
int shell_exit(std::vector<std::string> &args);
int shell_help(std::vector<std::string> &args);
int shell_type(std::vector<std::string> &args);
int shell_cmd_handler(std::vector<std::string> &args);

extern std::map<std::string, int(*)(std::vector<std::string>&)> shell_cmds;
