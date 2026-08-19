#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include "shell_cmds.hpp"
#include "utils.hpp"

std::map<std::string, int(*)(std::vector<std::string> &)> shell_cmds = {
    {"cd", shell_chdir},
    {"echo", shell_echo},
    {"exit", shell_exit},
    {"type", shell_type},
    {"pwd", shell_pwd},
    {"help", shell_help}
};

int shell_chdir(std::vector<std::string> &args) {
    std::string new_dir(args[1]);

    try {
        const char* home = nullptr;
        if(args[1][0] == '~') {
            home = std::getenv("USERPROFILE");
            fs::current_path(home);
        } else {
            fs::current_path(args[1]); // TODO: make 3 cases, one for absolute paths, one for dirs starting with . or .., one for ~
        }
    } catch (const fs::filesystem_error e) {
        std::cerr << "cd: " << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}

int shell_echo(std::vector<std::string> &args) {
    for(int i = 1; i < args.size(); i++) {
        std::cout << args[i] << " ";
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

int shell_exit(std::vector<std::string> &args) {
    std::cout << "Exiting WinShell..." << std::endl;
    exit(EXIT_SUCCESS);
}

int shell_help(std::vector<std::string> &args) {
    std::cout << "WinShell Help: " << std::endl;
    std::cout << "Write program names and arguments, and hit enter" << std::endl;
    std::cout << "The following commands are available by default:" << std::endl;
    for(auto it : shell_cmds) {
        std::cout << "  " << it.first << std::endl;
    }
    // TODO: Add man command print here once implemented
    return EXIT_SUCCESS;
}

int shell_pwd(std::vector<std::string> &args) {
    fs::path curr_dir = fs::current_path();
    std::string currPath = curr_dir.string();
    std::cout << currPath << std::endl;
    return EXIT_SUCCESS;
}

int shell_type(std::vector<std::string> &args) {
    int cmdType = SHELL_CMD_TYPE_UNKNOWN;
    const char* path_env = find_path_var();
    std::string path_string(path_env);
    std::stringstream ss(path_env);
    std::string directory, finaldir;

    for (auto it : shell_cmds) {
        if(args[1] == it.first) {
            cmdType = SHELL_CMD_TYPE_BUILTIN; // Built-in command
            break;
        }
    }
    if(cmdType == SHELL_CMD_TYPE_UNKNOWN) {
        std::string exec = args[1] + ".exe";
        while(std::getline(ss, directory, ';')) {
            fs::path target_path(directory);
            if(fs::exists(target_path) && fs::is_directory(target_path)) {
                for(const auto& entry : fs::directory_iterator(target_path)) {
                    if(fs::is_regular_file(entry) && entry.path().filename() == exec && execute_permission(entry.path())) {
                        cmdType = SHELL_CMD_TYPE_EXEC; // Executable
                        finaldir = directory;
                    }
                }
            }
        }
    }
    switch(cmdType) {
        case SHELL_CMD_TYPE_BUILTIN:
            std::cout << args[1] << " is a shell builtin." << std::endl;
            break;
        case SHELL_CMD_TYPE_EXEC:
            std::cout << args[1] << " is " << finaldir << std::endl;
            break;
        default:
            std::cout << "type: " << args[1] <<": not found" << std::endl;
    }
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
