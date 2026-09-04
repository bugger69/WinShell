#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include "shell_cmds.hpp"
#include "utils.hpp"

std::map<std::string, int(*)(std::vector<std::string> &, std::ostream*, std::ostream* err)> shell_cmds = {
    {"cd", shell_chdir},
    {"echo", shell_echo},
    {"exit", shell_exit},
    {"type", shell_type},
    {"pwd", shell_pwd},
    {"help", shell_help}
};

int shell_chdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
    std::string new_dir(args[1]);

    try {
        const char* home = nullptr;
        if(args[1][0] == '~') {
            home = std::getenv("USERPROFILE");
            std::string currPath(new_dir);
            currPath.erase(0, 1);
            std::string finalPath = home + currPath;
            fs::current_path(finalPath);
        } else {
            fs::current_path(new_dir);
        }
    } catch (const fs::filesystem_error e) {
        *err << "cd: " << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}

int shell_echo(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
    for(int i = 1; i < args.size(); i++) {
        *out << args[i] << " ";
    }
    *out << std::endl;
    return EXIT_SUCCESS;
}

int shell_exit(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
    *out << "Exiting WinShell..." << std::endl;
    exit(EXIT_SUCCESS);
}

int shell_help(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
    *out << "WinShell Help: " << std::endl;
    *out << "Write program names and arguments, and hit enter" << std::endl;
    *out << "The following commands are available by default:" << std::endl;
    for(auto it : shell_cmds) {
        *out << "  " << it.first << std::endl;
    }
    // TODO: Add man command print here once implemented
    return EXIT_SUCCESS;
}

int shell_pwd(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
    fs::path curr_dir = fs::current_path();
    std::string currPath = curr_dir.string();
    *out << currPath << std::endl;
    return EXIT_SUCCESS;
}

int shell_type(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
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
            *out << args[1] << " is a shell builtin." << std::endl;
            break;
        case SHELL_CMD_TYPE_EXEC:
            *out << args[1] << " is " << finaldir << std::endl;
            break;
        default:
            *out << "type: " << args[1] <<": not found" << std::endl;
    }
    return EXIT_SUCCESS;
}

int shell_cmd_handler(std::vector<std::string> &args, std::ostream* out, std::ostream* err) {
    if (args.empty()) return EXIT_FAILURE;
    for(auto it : shell_cmds) {
        if(args[0] == it.first) {
            return it.second(args, out, err);
        }
    }
    return EXIT_FAILURE;
}
