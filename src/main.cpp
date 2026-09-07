#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "shell_cmds.hpp"
#include "utils.hpp"

void setConsoleOutputBuf(std::vector<std::string> &args, OutputTarget &consoleOut) {
    std::vector<int> toErase;
    std::vector<std::string> temp;
    
    for(int i = 1; i < args.size(); i++) {
        if(i > 0 && (args[i - 1] == ">" || args[i - 1] == "1>") && (args[i] != ">" || args[i] != "1>")) {
            consoleOut.flag_std = SHELL_CMD_OUT_STD_FILE;
            consoleOut.file_std = args[i];
            toErase.push_back(i - 1);
            toErase.push_back(i);
        } else if (i > 0 && (args[i - 1] == ">" || args[i - 1] == "1>") && (args[i] == ">" || args[i] == "1>")) {
            toErase.push_back(i - 1);
        } else if(i > 0 && (args[i - 1] == "2>") && (args[i] != "2>")) {
            consoleOut.flag_err = SHELL_CMD_OUT_ERR_FILE;
            consoleOut.file_err = args[i];
            toErase.push_back(i - 1);
            toErase.push_back(i);
        } else if (i > 0 && (args[i - 1] == "2>") && (args[i] == "2>")) {
            toErase.push_back(i - 1);
        } else if (i > 0 && (args[i - 1] == ">>") && (args[i] != ">>")) {
            consoleOut.flag_std = SHELL_CMD_OUT_STD_APPEND;
            consoleOut.file_std = args[i];
            toErase.push_back(i - 1);
            toErase.push_back(i);
        } else if (i > 0 && (args[i - 1] == ">>") && (args[i] == ">>")) {
            toErase.push_back(i - 1);
        } else if (i > 0 && (args[i - 1] == "2>>") && (args[i] != "2>>")) {
            consoleOut.flag_err = SHELL_CMD_OUT_ERR_APPEND;
            consoleOut.file_err = args[i];
            toErase.push_back(i - 1);
            toErase.push_back(i);
        } else if (i > 0 && (args[i - 1] == "2>>") && (args[i] == "2>>")) {
            toErase.push_back(i - 1);
        }
    }
    
    for(int i = 0; i < args.size(); i++) {
        if(find(toErase.begin(), toErase.end(), i) != toErase.end()) continue;
        temp.push_back(args[i]);
    }

    args.clear();
    for(auto it : temp) {
        args.push_back(it);
    }
}

std::string shell_find_exec(std::vector<std::string> &args, int &found) { // TODO: Add support for full file paths
    std::string exec = !endsWith(args[0], ".exe") ? args[0] + ".exe" : args[0];
    const char* path_env = find_path_var();
    std::string path_string(path_env), local_execs(EXEC_BIN_PATH), exec_path;
    path_string = local_execs + ';' + path_string;
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

int shell_process_launch(std::vector<std::string> &args, STARTUPINFOW &si, PROCESS_INFORMATION &pi, OutputTarget &consoleOut, std::ostream* out, std::ostream* err)
{
    int cmd_found = EXIT_FAILURE;
    std::wstring command;
    std::string execPath = shell_find_exec(args, cmd_found);
    SECURITY_ATTRIBUTES sa{}; // create security attributes to enable inheritance
    HANDLE outputHandle = INVALID_HANDLE_VALUE;
    HANDLE errHandle = INVALID_HANDLE_VALUE;

    if (out != &std::cout) {  // check if filestream is opn, closes it in that case
        if (auto* fileStream = dynamic_cast<std::ofstream*>(out)) {
            fileStream->close();
        }
    }

    if(err != &std::cerr) {
        if (auto* fileStream = dynamic_cast<std::ofstream*>(err)) {
            fileStream->close();
        }
    }

    if(cmd_found == EXIT_FAILURE) return EXIT_FAILURE;

    if (consoleOut.flag_std == SHELL_CMD_OUT_STD_FILE || consoleOut.flag_std == SHELL_CMD_OUT_STD_APPEND) {
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        
        std::wstring filename( // create windows string
            consoleOut.file_std.begin(),
            consoleOut.file_std.end()
        );

        outputHandle = CreateFileW( // create file Write Handler
            filename.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_WRITE, // gotta figure out how this works
            &sa,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (outputHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create output file\n" << std::endl;
            return EXIT_SUCCESS;
        } else {
            si.dwFlags |= STARTF_USESTDHANDLES;
            si.hStdOutput = outputHandle;
        }
    }

    if (consoleOut.flag_err == SHELL_CMD_OUT_ERR_FILE || consoleOut.flag_err == SHELL_CMD_OUT_ERR_APPEND) {
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        sa.lpSecurityDescriptor = NULL;
        
        std::wstring filename( // create windows string
            consoleOut.file_err.begin(),
            consoleOut.file_err.end()
        );

        errHandle = CreateFileW( // create file Write Handler
            filename.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_WRITE, // gotta figure out how this works
            &sa,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (errHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create error file\n" << std::endl;
            return EXIT_SUCCESS;
        } else {
            si.dwFlags |= STARTF_USESTDHANDLES;
            // si.hStdOutput = outputHandle;
            si.hStdError = errHandle;
        }
    }

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
        TRUE,                  // Set handle inheritance to TRUE
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
        CloseHandle(outputHandle); // closing output handle 
        CloseHandle(errHandle); // closing error handle
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Error: CreateProcess failed with error code " << GetLastError() << std::endl;
    }

    return EXIT_SUCCESS;
}

int shell_execute(std::vector<std::string> &args, OutputTarget &consoleOut) // TODO: add basic commands
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    int status;
    std::ofstream file, errFile;
    std::ostream* out = &std::cout;
    std::ostream* err = &std::cerr;

    if(args.empty()) goto end;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    setConsoleOutputBuf(args, consoleOut);

    if(consoleOut.flag_std == SHELL_CMD_OUT_STD_FILE || consoleOut.flag_std == SHELL_CMD_OUT_STD_APPEND) {
        if(consoleOut.file_std == "") goto end;
        consoleOut.flag_std == SHELL_CMD_OUT_STD_FILE ? file.open(consoleOut.file_std) : file.open(consoleOut.file_std, std::ios::app);
        if (!file) {
            std::cerr << "Failed to open output file" << std::endl;
            goto end;
        }
        out = &file;
    }

    if(consoleOut.flag_err == SHELL_CMD_OUT_ERR_FILE || consoleOut.flag_err == SHELL_CMD_OUT_ERR_APPEND) {
        if(consoleOut.file_err == "") goto end;
        consoleOut.flag_err == SHELL_CMD_OUT_ERR_FILE ? errFile.open(consoleOut.file_err) : errFile.open(consoleOut.file_err, std::ios::app);
        if (!errFile) {
            std::cerr << "Failed to open output file" << std::endl;
            goto end;
        }
        err = &errFile;
    }
    // TODO: Write code for all shell commands, and add support for more commands later
    // IF IT'S NOT A SHELL COMMAND, THEN LAUNCH IT AS A PROCESS 
    if(shell_cmd_handler(args, out, err) == EXIT_FAILURE && shell_process_launch(args, si, pi, consoleOut, out, err) == EXIT_FAILURE) {
        std::cerr << args[0] << ": command not found" << std::endl;
    }

end: // TODO: Reset consoleout here
    consoleOut.flag_std = SHELL_CMD_OUT_STD_DEF;
    consoleOut.file_std = "";
    consoleOut.flag_err = SHELL_CMD_OUT_ERR_DEF;
    consoleOut.file_err = "";
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
    OutputTarget consoleOut;
    int status;

    do
    {
        std::cout << "> ";
        std::getline(std::cin, line);
        shell_parse(line, args);
        status = shell_execute(args, consoleOut);

        line.clear();
        args.clear();
        // std::cin.ignore();
    } while (!status);
}
// TODO: implement type and echo commands too
int main()
{

    shell_loop(); // TODO: fix the > and 1> operators not working errors, try replacing cout with out..

    return EXIT_SUCCESS;
}