#include <iostream>
#include <string>
#include <vector>
#include <map>

int shell_chdir(std::vector<std::string> &args);
int shell_exit(std::vector<std::string> &args);
int shell_help(std::vector<std::string> &args);
int shell_cmd_handler(std::vector<std::string> &args);

extern std::map<std::string, int(*)(std::vector<std::string>&)> shell_cmds;
