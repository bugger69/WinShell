#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "shell_context.hpp"

// Shell Status
#define SHELL_STATUS_NORM 0
#define SHELL_STATUS_BREAK 1
#define SHELL_STATUS_CONTINUE 2

// Type Headers
#define SHELL_CMD_TYPE_UNKNOWN -1
#define SHELL_CMD_TYPE_BUILTIN 0
#define SHELL_CMD_TYPE_EXEC 1

// Default executables array
#define EXEC_BIN_PATH "./modules/"


int shell_chdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_complete(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_echo(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_exit(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_help(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_jobs(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_listdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_pwd(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_type(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);
int shell_cmd_handler(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context);

extern std::map<std::string, int(*)(std::vector<std::string> &, std::ostream*, std::ostream*, ShellContext &)> shell_cmds;
