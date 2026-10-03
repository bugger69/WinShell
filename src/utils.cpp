#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include "Trie.hpp"
#include "utils.hpp"

bool has_space(const std::string& cmd) {
    for(auto it : cmd) {
        if(it == ' ') return true;
    }
    return false;
}

bool endsWith(const std::string& mainStr, const std::string& suffix) {
    if (mainStr.length() < suffix.length()) return false;
    return mainStr.rfind(suffix) == (mainStr.length() - suffix.length());
}

const char* find_path_var() {
    const char* path_env = std::getenv("PATH");
    if(!path_env) {
        std::cout << "WinShell: unable to find path." << std::endl;
    }
    return path_env;
}

bool execute_permission(const fs::path exec_path) {
    if(!fs::exists(exec_path)) {
        return false;
    }
    fs::perms p = fs::status(exec_path).permissions();
    return ((p & EXEC_PERMISSIONS) != fs::perms::none);
}

bool isEscape(const std::string &line, int i) {
    if(i > 0 && line[i - 1] == '\\') return true;
    return false;
}

bool isCurrEscape(const std::string &line, int i) {
    if(i > 0 && line[i - 1] != '\\' && line[i] == '\\') return true;
    return false;
}

bool isDoubleQuoteSp(const std::string &line, int i) {
    for(auto it : DOUBLEQUOTESPCHARS) {
        if(i < line.size() - 1 && line[i] == '\\' && line[i + 1] == it) {
            return true;
        }
    }
    return false;
}

bool startsWith(const std::string& mainStr, const std::string& prefix) {
    if(prefix.size() > mainStr.size()) return false;
    return mainStr.find(prefix) == 0;
}

std::string remove_start(const std::string &cmd, std::string& prefix) {
    if(startsWith(cmd, prefix)) {
        std::string str(cmd.begin() + prefix.size(), cmd.end());
        return str;
    }
    return cmd;
}

std::string remove_end(const std::string &cmd, std::string& suffix) {
    if(endsWith(cmd, suffix)) {
        std::string str(cmd.begin(), cmd.end() - suffix.size());
        return str;
    }
    return cmd;
}

std::string common_prefix(const std::string &str1, const std::string &str2) {
    std::size_t k = 0;
    std::string ans;
    if(str1.size() > str2.size()) {
        k = str2.size();
    } else {
        k = str1.size();
    }
    for(int i = 0; i < k; i++) {
        if(str1[i] == str2[i]) {
            ans += str1[i];
        } else break;
    }
    return ans;
}

int populate_dir(Trie* curr, fs::path &cwd) {
    if(fs::is_directory(cwd)) {
        curr->clear();
        for(const auto& entry : fs::directory_iterator(cwd)) {
            std::string currpath = entry.path().string();
            if(fs::is_directory(currpath)) {
                std::string dir = entry.path().filename().string();
                curr->insert(dir, "directory");
            } else if(fs::is_regular_file(currpath)) {
                std::string file = entry.path().filename().string();
                curr->insert(file, "file");
            }
        }
    }
    return 0;
}

ProcessResult getOutputFromProcess(const std::wstring& command) {
    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;

    /* Creating a Pipe */
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        throw std::runtime_error("CreatePipe failed");
    }

    if (!SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        throw std::runtime_error("SetHandleInformation failed");
    }

    /* Startup Info for Process */
    STARTUPINFOW si{};
    si.cb = sizeof(STARTUPINFOW);

    si.dwFlags |= STARTF_USESTDHANDLES;

    si.hStdOutput = writePipe;
    si.hStdError = writePipe;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    /* Creating the Process */
    PROCESS_INFORMATION pi{};
    std::vector<wchar_t> cmd(command.begin(), command.end());
    cmd.push_back(L'\0');

    BOOL success = CreateProcessW(
        nullptr,        // Application name
        cmd.data(),     // Command line
        nullptr,        // Process security attributes
        nullptr,        // Thread security attributes
        TRUE,           // Inherit handles
        0,              // Creation flags
        nullptr,        // Environment
        nullptr,        // Current directory
        &si,
        &pi
    );

    if (!success) {
        CloseHandle(readPipe);
        CloseHandle(writePipe);

        throw std::runtime_error("CreateProcessW failed");
    }

    /* close write */
    CloseHandle(writePipe);
    writePipe = nullptr;

    /* Getting Output */
    ProcessResult Out;
    char buffer[4096];
    DWORD bytesRead;

        while (true) {
        BOOL success = ReadFile(
            readPipe,
            buffer,
            sizeof(buffer),
            &bytesRead,
            nullptr
        );

        if (!success || bytesRead == 0) {
            break;
        }

        Out.output.append(buffer, bytesRead);
    }

    CloseHandle(readPipe);

    /* Wait for Object */
    WaitForSingleObject(pi.hProcess, INFINITE);

    /* Exit Code */
    GetExitCodeProcess(
        pi.hProcess,
        &Out.exitCode
    );

    /* Closing Handles */
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return Out;
}