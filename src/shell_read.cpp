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
#include "shell_read.hpp"
#include "utils.hpp"

constexpr int TAB_SIZE = 4;

std::string longestCommonPrefix(std::vector<std::string>& cmdList, std::string& cmd) {
    std::string longestPrefix = cmdList[0]; // TODO: change this to find longest common prefix from cmdList
    std::size_t size = cmdList.size();
    for(int i = 0; i < size - 1; i++) {
        longestPrefix = common_prefix(longestPrefix, cmdList[i + 1]);
    }
    return longestPrefix;
}

void findCommands(std::vector<std::string>& cmdList, std::string& prefix) {
    const char* path_env = find_path_var();
    std::string path_string(path_env), local_execs(EXEC_BIN_PATH), exec_path;
    path_string = local_execs + ';' + path_string;
    std::stringstream ss(path_string);
    std::string directory, execSuffix = ".exe";

    for(auto it : shell_cmds) {
        if(startsWith(it.first, prefix)) {
            cmdList.push_back(it.first);
        }
    }

    while(std::getline(ss, directory, ';')) {
        fs::path target_path(directory);
        if(fs::exists(target_path) && fs::is_directory(target_path)) {
            for(const auto& entry : fs::directory_iterator(target_path)) {
                if(fs::is_regular_file(entry) && execute_permission(entry.path())) {
                    std::string filename = entry.path().filename().string();
                    if(endsWith(filename, execSuffix) && startsWith(filename, prefix)) {
                        filename = remove_end(filename, execSuffix);
                        cmdList.push_back(filename);
                    }
                }
            }
        }
    }

    sort(cmdList.begin(), cmdList.end());
}

void handle_autocomplete(std::string &cmd) {
    std::vector<std::string> allCmds;
    findCommands(allCmds, cmd);
    if(allCmds.size() == 0) {
        std::cout << '\x07';
    } else if (allCmds.size() == 1) {
        std::string str = remove_start(allCmds[0], cmd);
        for(auto it : str) {
            cmd.push_back(it);
            std::cout << it;
        }
        cmd.push_back(' ');
        std::cout << ' ';
    } else {
        std::string curr = longestCommonPrefix(allCmds, cmd);
        std::string diff = remove_start(curr, cmd);
        std::size_t columnWidth = 0;
        int i = 1;
        std::cout << '\n';
        for (const auto& command : allCmds) {
            columnWidth = (std::max)(columnWidth, command.size());
        }

        columnWidth += 4; // spacing between columns

        for (std::size_t i = 0; i < allCmds.size(); ++i) {
            std::cout << std::left << std::setw(static_cast<int>(columnWidth))
                    << allCmds[i];

            if ((i + 1) % 3 == 0) {
                std::cout << '\n';
            }
        }

        if (allCmds.size() % 3 != 0) {
            std::cout << '\n';
        }
        std::cout << '>';
        std::cout << ' ';
        std::cout << curr;
        cmd += diff;
        i = 0;
    }
}



void shell_read(std::string &line, int &status) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD prevHandle;
    GetConsoleMode(hIn, &prevHandle);

    SetConsoleMode(hIn, prevHandle & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

    std::string inputBuffer;
    INPUT_RECORD ir;
    DWORD read;

    while(true) {
        ReadConsoleInput(hIn, &ir, 1, &read);

        if(ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            auto &ke = ir.Event.KeyEvent;
            WORD vk = ke.wVirtualKeyCode;
            char ch = ke.uChar.AsciiChar;

            if(vk == VK_ESCAPE) break;

            if(vk == VK_BACK) {
                if(!inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    std::cout<< "\b \b" << std::flush;
                }
            } else if (vk == VK_TAB) {
                if (!has_space(inputBuffer)) {
                    handle_autocomplete(inputBuffer);
                } else {
                    int spacesToAdd = TAB_SIZE - (static_cast<int>(inputBuffer.size()) % TAB_SIZE);
                    for (int i = 0; i < spacesToAdd; ++i) {
                        inputBuffer.push_back(' ');
                        std::cout << ' ';
                    }
                }
                std::cout << std::flush;
            } else if (vk == VK_RETURN) {
                line = inputBuffer;
                std::cout << std::endl;
                inputBuffer.clear();
                break;
            } else if (ch >= 32 && ch <= 126) {
                inputBuffer.push_back(ch);
                std::cout<<ch<<std::flush;
            }
        }
    }
    SetConsoleMode(hIn, prevHandle);
}
