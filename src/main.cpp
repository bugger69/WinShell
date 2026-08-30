#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include "shell_cmds.hpp"
#include "utils.hpp"

std::string shell_find_exec(std::vector<std::string> &args, int &found) { // TODO: Add support for full file paths
    std::string exec = args[0] + ".exe";
    const char* path_env = find_path_var();
    std::string path_string(path_env), exec_path;
    std::stringstream ss(path_string);
    std::string directory, finaldir;

    while(std::getline(ss, directory, ';')) {
        fs::path target_path(directory);
        if(fs::exists(target_path) && fs::is_directory(target_path)) {
            for(const auto& entry : fs::directory_iterator(target_path)) {
                if(fs::is_regular_file(entry) && entry.path().filename() == exec && execute_permission(entry.path())) {
                    finaldir = directory;
                    found = EXIT_SUCCESS;
                }
            }
        }
    }

    if(finaldir.size()) exec_path = finaldir + '/' + exec;

    return exec_path;
}

int shell_process_launch(std::vector<std::string> &args, STARTUPINFOW &si, PROCESS_INFORMATION &pi) // TODO: Use windows CreateProcess and CreateThread to implement process forking + exec.
{
    int cmd_found = EXIT_FAILURE;
    std::wstring command;
    std::string execPath = shell_find_exec(args, cmd_found);

    if(cmd_found == EXIT_FAILURE) return EXIT_FAILURE;

    for (int i = 0; i < args.size(); i++) {
        std::string arg = args[i];
        if(i == 0) {
            command += std::wstring(execPath.begin(), execPath.end()) + L" ";
        } else {
            command += std::wstring(arg.begin(), arg.end()) + L" ";
        }
    }

    bool status = CreateProcessW(
        NULL,                   // No module name (use command line)
        command.data(),         // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        0,                      // No creation flags
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's starting directory 
        &si,                    // Pointer to STARTUPINFO structure
        &pi                     // Pointer to PROCESS_INFORMATION structure
    );

    if(status) {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exit_code;
        if (GetExitCodeProcess(pi.hProcess, &exit_code)) {
            std::cout << "Process exited with code: " << exit_code << std::endl;
        } else {
            std::cerr << "Error: GetExitCodeProcess failed with error code " << GetLastError() << std::endl;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Error: CreateProcess failed with error code " << GetLastError() << std::endl;
    }

    return EXIT_SUCCESS;
}

int shell_execute(std::vector<std::string> &args) // TODO: add basic commands
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    int status;

    if(args.empty()) goto end;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));


    // TODO: Write code for all shell commands, and add support for more commands later
    // IF IT'S NOT A SHELL COMMAND, THEN LAUNCH IT AS A PROCESS  
    if(shell_cmd_handler(args) == EXIT_FAILURE && shell_process_launch(args, si, pi) == EXIT_FAILURE) {
        std::cerr << args[0] << ": command not found" << std::endl;
    }

end:
    return EXIT_SUCCESS;
}

void shell_parse(const std::string &line, std::vector<std::string> &args)
{
    int i = 0;
    std::string curr;
    while(i < line.size()) {
        if(!isEscape(line, i) && line[i] == '\"') {
            i++;
            while(line[i] != '\"') {
                if(isDoubleQuoteSp(line, i)) {
                    curr += line[i + 1];
                    i += 2;
                } else {
                    curr += line[i];
                    i++;
                }
               
            }
            i++;
            continue;
        }
        if(!isEscape(line, i) && line[i] == '\'') {
            i++;
            while(line[i] != '\'') {
                curr += line[i];
                i++;
            }
            i++;
            continue;
        }
        if(!isEscape(line, i) && line[i] == ' ') {
            if(!curr.empty()) args.push_back(curr);
            curr = "";
            i++;
            continue;
        }
        if(!isCurrEscape(line, i)) curr += line[i];
        i++;
    }
    if(!curr.empty()) args.push_back(curr);
    curr = "";
}

void shell_loop(void)
{
    std::string line;
    std::vector<std::string> args;
    int status;

    do
    {
        std::cout << "> ";
        std::getline(std::cin, line);
        shell_parse(line, args);
        status = shell_execute(args);

        line.clear();
        args.clear();
        // std::cin.ignore();
    } while (!status);
}
// TODO: implement type and echo commands too
int main()
{

    shell_loop(); // TODO: add modules, and to that add a basic hello executable, and add the path to that to the path string too.
    
    return EXIT_SUCCESS;
}