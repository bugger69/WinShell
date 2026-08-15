#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "shell_cmds.hpp"

std::map<std::string, int(*)(std::vector<std::string> &)> shell_cmds = {
    {"cd", shell_chdir},
    {"exit", shell_exit},
    {"help", shell_help}
};

int shell_chdir(std::vector<std::string> &args) {
    std::cout << "Changing directory to: " << args[1] << std::endl;
    return EXIT_SUCCESS;
}

int shell_exit(std::vector<std::string> &args) {
    std::cout << "Exiting WinShell..." << std::endl;
    exit(EXIT_SUCCESS);
}

int shell_help(std::vector<std::string> &args) {
    std::cout << "shell_help called" << std::endl;
    return EXIT_SUCCESS;
}

int shell_cmd_handler(std::vector<std::string> &args) {
    if (args.empty()) return EXIT_FAILURE;
    for(auto it : shell_cmds) {
        if(args[0] == it.first) {
            return it.second(args);
        }
    }
    return EXIT_FAILURE;
}
