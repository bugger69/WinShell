#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "shell_read.hpp"
#include "parser.hpp"
#include "shell_execute.hpp"
#include "shell_cmds.hpp"
#include "utils.hpp"
#include "Trie.hpp"

ShellContext shell_init() {
    ShellContext context;
    const char* path_env = find_path_var();
    std::string path_string(path_env), local_execs(EXEC_BIN_PATH), exec_path;
    path_string = local_execs + ';' + path_string;
    std::stringstream ss(path_string);
    std::string directory, finaldir;
    context.path = new Trie();
    while(std::getline(ss, directory, ';')) {
        fs::path target_path(directory);
        if(fs::exists(target_path) && fs::is_directory(target_path)) {
            for(const auto& entry : fs::directory_iterator(target_path)) {
                std::string exec = entry.path().filename().string();
                std::string suffix = ".exe"; // The only one supported for now
                if(execute_permission(entry.path()) && endsWith(exec, suffix)) {
                    std::string cmd = remove_end(exec, suffix);
                    std::string filePath = entry.path().string();
                    context.path->insert(cmd, filePath);
                }
            }
        }
    }
    return context;
}

void shell_loop(ShellContext& context)
{
    std::string line;
    std::vector<std::string> args;
    OutputTarget consoleOut;
    int status = SHELL_STATUS_NORM;

    do
    {
        std::cout << "> ";
        shell_read(line, status, context);
        shell_parse(line, args, context);
        status = shell_execute(args, consoleOut, context);

        line.clear();
        args.clear();
        // std::cin.ignore();
    } while (!status);
}
// TODO: implement type and echo commands too
int main()
{
    ShellContext context = shell_init();

    shell_loop(context); // TODO: Add the handler for autocomplete command

    return EXIT_SUCCESS;
}