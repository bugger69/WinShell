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

void handle_autocomplete(std::string &cmd) {
    std::vector<std::string> allCmds;
    for(auto it : shell_cmds) {
        if(startsWith(it.first, cmd)) {
            allCmds.push_back(it.first);
        }
    }
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
        std::cout << "\n";
        for(auto it : allCmds) {
            std::cout << it;
            std::cout << '\n';
        }
        std::cout << '>';
        std::cout << ' ';
        std::cout << cmd;
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
