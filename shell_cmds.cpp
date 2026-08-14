#include <iostream>
#include <string>
#include <vector>
#include "shell_cmds.hpp"

std::vector<std::string> shell_cmds = {
    "cd",
    "exit",
    "help"
};

int shell_chdir(std::vector<std::string> &args) {
    return EXIT_SUCCESS;
}

int shell_exit(std::vector<std::string> &args) {
    std::cout << "Exiting WinShell..." << std::endl;
    exit(EXIT_SUCCESS);
}

int shell_help(std::vector<std::string> &args) {
    return EXIT_SUCCESS;
}

int shell_cmd_handler(std::vector<std::string> &args) {
    if (args.empty()) return EXIT_FAILURE;
    for(auto it : shell_cmds) {
        if(args[0] == it) {
            if(it == "cd") {
                return shell_chdir(args);
            } else if(it == "exit") {
                return shell_exit(args);
            } else if(it == "help") {
                return shell_help(args);
            }
        }
    }
    return EXIT_SUCCESS;
}
