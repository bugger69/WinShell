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
    fs::path curr_dir = fs::current_path();
    const char* path_env = find_path_var();
    std::string path_string(path_env), local_execs(EXEC_BIN_PATH), exec_path;
    path_string = local_execs + ';' + path_string;
    std::stringstream ss(path_string);
    std::string directory, finaldir;
    context.path = new Trie();
    context.CurrDir = new Trie();
    context.newDir = new Trie();
    context.tabSM = new Tabsm();

    /* Populating current directory */
    populate_dir(context.CurrDir, curr_dir);

    /* Populating new directory trie */
    context.newDir = new Trie(*context.CurrDir);

    /* Intitializing Tab state machine */
    context.tabSM->curr = TAB_AUTOCOMP_CMD;
    context.tabSM->count = 0;
    context.tabSM->pathsPossible = true;

    /* Initializing Shell Commands */
    for(auto it : shell_cmds) {
        context.path->insert(it.first, "shell-command");
    }

    /* Initialising Jobs */
    context.jobs = new Jobs();
    context.jobs->processCount = 0;
    context.jobs->nextBuiltinJobId = 1;

    /* Initialising output target */
    context.consoleOut = new OutputTarget();

    /* Initializing path */
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

    /* Initializing History environment variable */
    const char* histfile = std::getenv("HISTFILE");
    if(histfile == nullptr) {
        if(_putenv_s("HISTFILE", DEF_HIST_FILE) != 0) {
            std::cerr << "Unable to set history file\n" << std::flush;
        } else {
            histfile = std::getenv("HISTFILE");
        }
    }
    if(!fs::exists(histfile)) {
        std::ofstream hist(histfile);
        hist.close();
    }
    if(fs::exists(histfile)) {
        std::ifstream hist(histfile);
        std::string line;
        while(getline(hist, line)) {
            context.history.push_back(line);
        }
    }
    
    return context;
}

void shell_loop(ShellContext& context)
{
    std::string line;
    std::vector<std::string> args;
    int status = SHELL_STATUS_NORM;

    do
    {
        std::cout << "> ";
        shell_read(line, status, context);
        shell_parse(line, args, context);
        status = shell_execute(args, context);

        line.clear();
        args.clear();
        // std::cin.ignore();
    } while (!status);
}
// TODO: implement type and echo commands too
int main()
{
    ShellContext context = shell_init();

    shell_loop(context);
    return EXIT_SUCCESS;
}