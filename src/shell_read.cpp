#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "shell_context.hpp"
#include "shell_cmds.hpp"
#include "shell_read.hpp"
#include "utils.hpp"

constexpr int TAB_SIZE = 4;

void handle_autocomplete(std::string &cmd, ShellContext &context, bool &waitingForSecondTab) { // TODO: optimise further when you add in shell commands
    std::string extCmd = context.path->extendPrefix(cmd);
    if(extCmd == cmd && !(context.path->search(extCmd))) { // TODO: Add support for if a command is a substring of another command (one more else if)
        std::vector<std::string> allCmds = context.path->allCmdsFromPrefix(cmd);
        if(allCmds.size() == 0) {
            std::cout << '\x07';
        } else if (waitingForSecondTab) {
            std::string diff = remove_start(extCmd, cmd);
            std::size_t columnWidth = 0;
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
            std::cout << extCmd;
            cmd += diff;
        } else {
            waitingForSecondTab = !waitingForSecondTab;
        }
    } else {
        std::string str = remove_start(extCmd, cmd);
        for(auto it : str) {
            cmd.push_back(it);
            std::cout << it;
        }
        cmd.push_back(' ');
        std::cout << ' ';
        waitingForSecondTab = false;
    }
}



void shell_read(std::string &line, int &status, ShellContext &context) {
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD prevHandle;
    GetConsoleMode(hIn, &prevHandle);

    SetConsoleMode(hIn, prevHandle & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

    std::string inputBuffer;
    INPUT_RECORD ir;
    DWORD read;
    bool waitingForSecondTab = false;

    while(true) {
        ReadConsoleInput(hIn, &ir, 1, &read);

        if(ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            auto &ke = ir.Event.KeyEvent;
            WORD vk = ke.wVirtualKeyCode;
            char ch = ke.uChar.AsciiChar;

            if(vk == VK_ESCAPE) break;

            if(vk != VK_TAB) waitingForSecondTab = false;

            if(vk == VK_BACK) {
                if(!inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    std::cout<< "\b \b" << std::flush;
                }
            } else if (vk == VK_TAB) { // TODO: Add support for two tabs
                if (!has_space(inputBuffer)) {
                    handle_autocomplete(inputBuffer, context, waitingForSecondTab);
                } else if (!waitingForSecondTab) {
                    int spacesToAdd = TAB_SIZE - (static_cast<int>(inputBuffer.size()) % TAB_SIZE);
                    for (int i = 0; i < spacesToAdd; ++i) {
                        inputBuffer.push_back(' ');
                        std::cout << ' ';
                    }
                } else {
                    waitingForSecondTab = false;
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
