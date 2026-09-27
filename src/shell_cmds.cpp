#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include "shell_cmds.hpp"
#include "utils.hpp"

std::map<std::string, int(*)(std::vector<std::string> &, std::ostream*, std::ostream*, ShellContext &)> shell_cmds = {
    {"cd", shell_chdir},
    {"echo", shell_echo},
    {"exit", shell_exit},
    {"type", shell_type},
    {"pwd", shell_pwd},
    {"help", shell_help}
};


int shell_chdir(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
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

int shell_echo(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    for(int i = 1; i < args.size(); i++) {
        *out << args[i] << " ";
    }
    *out << std::endl;
    return EXIT_SUCCESS;
}

int shell_exit(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    *out << "Exiting WinShell..." << std::endl;
    exit(EXIT_SUCCESS);
}

int shell_help(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    *out << "WinShell Help: " << std::endl;
    *out << "Write program names and arguments, and hit enter" << std::endl;
    *out << "The following commands are available by default:" << std::endl;
    for(auto it : shell_cmds) {
        *out << "  " << it.first << std::endl;
    }
    // TODO: Add man command print here once implemented
    return EXIT_SUCCESS;
}

int shell_pwd(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    fs::path curr_dir = fs::current_path();
    std::string currPath = curr_dir.string();
    *out << currPath << std::endl;
    return EXIT_SUCCESS;
}

int shell_type(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    int cmdType = SHELL_CMD_TYPE_UNKNOWN;
    bool isCommand = context.path->search(args[1]);
    std::string directory, finaldir;
    if(isCommand) {
        std::vector<std::string> exec = context.path->getExec(args[1]);
        if(exec[0] == "shell-command") {
            cmdType = SHELL_CMD_TYPE_BUILTIN;
        } else {
            cmdType = SHELL_CMD_TYPE_EXEC;
            directory = exec[0];
            std::string exe;
            for(int i = directory.size() - 1; i >= 0; i--) {
                if(directory[i] == '/' || directory[i] == '\\') break;
                exe += directory[i];
            }
            reverse(exe.begin(), exe.end());
            finaldir = remove_end(directory, exe);
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

int shell_cmd_handler(std::vector<std::string> &args, std::ostream* out, std::ostream* err, ShellContext &context) {
    if (args.empty()) return EXIT_FAILURE;

    if(!(shell_cmds.find(args[0]) == shell_cmds.end())) // TODO: replace this line with trie search?
        return shell_cmds[args[0]](args, out, err, context);

    return EXIT_FAILURE;
}
