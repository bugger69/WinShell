#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include "shell_cmds.hpp"

#define DELIMITERS " \t\r\n\a"

int shell_process_launch(std::vector<std::string> &args, STARTUPINFOW &si, PROCESS_INFORMATION &pi) // TODO: Use windows CreateProcess and CreateThread to implement process forking + exec.
{
    std::wstring command;
    for (const auto &arg : args) {
        command += std::wstring(arg.begin(), arg.end()) + L" ";
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
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    status = shell_cmd_handler(args); // TODO: Write code for all shell commands, and add support for more commands later
    // IF IT'S NOT A SHELL COMMAND, THEN LAUNCH IT AS A PROCESS
    if(status == EXIT_FAILURE) {
        status = shell_process_launch(args, si, pi);
    }

    return EXIT_SUCCESS;
}

void shell_parse(const std::string &line, std::vector<std::string> &args)
{
    std::stringstream line_stream(line);
    std::string arg;
    while(std::getline(line_stream, arg, DELIMITERS[0])) { // TODO: Add support for quoted arguments, and other delimiters later (using string view)
        if(!arg.empty()) {
            args.push_back(arg);
        }
    }
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

int main()
{

    shell_loop(); // TODO: Test if all functions are working properly after changing the table to map

    return EXIT_SUCCESS;
}