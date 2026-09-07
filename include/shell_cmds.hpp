#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

// Type Headers
#define SHELL_CMD_TYPE_UNKNOWN -1
#define SHELL_CMD_TYPE_BUILTIN 0
#define SHELL_CMD_TYPE_EXEC 1

// Default executables array
#define EXEC_BIN_PATH "./modules/"

// Output macros
#define SHELL_CMD_OUT_STD_DEF 0
#define SHELL_CMD_OUT_STD_FILE 1
#define SHELL_CMD_OUT_STD_APPEND 2
#define SHELL_CMD_OUT_ERR_DEF 0
#define SHELL_CMD_OUT_ERR_FILE 1
#define SHELL_CMD_OUT_ERR_APPEND 2


class OutputTarget {
    public:
        int flag_std = SHELL_CMD_OUT_STD_DEF;
        int flag_err = SHELL_CMD_OUT_ERR_DEF;
        std::string file_std;
        std::string file_err;
};

int shell_chdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err);
int shell_echo(std::vector<std::string> &args, std::ostream* out, std::ostream* err);
int shell_exit(std::vector<std::string> &args, std::ostream* out, std::ostream* err);
int shell_help(std::vector<std::string> &args, std::ostream* out, std::ostream* err);
int shell_pwd(std::vector<std::string> &args, std::ostream* out, std::ostream* err);
int shell_type(std::vector<std::string> &args, std::ostream* out, std::ostream* err);
int shell_cmd_handler(std::vector<std::string> &args, std::ostream* out, std::ostream* err);

extern std::map<std::string, int(*)(std::vector<std::string>&, std::ostream*, std::ostream* err)> shell_cmds;
