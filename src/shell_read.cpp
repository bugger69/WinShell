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

void handleAutocompeteCmd(std::string &cmd, ShellContext &context) {
    std::string extCmd = context.path->extendPrefix(cmd);
    std::vector<std::string> allCmds = context.path->allCmdsFromPrefix(cmd);
    if((extCmd == cmd && !(context.path->search(extCmd))) || (allCmds.size() > 1)) {
        if(allCmds.size() == 0 && context.tabSM->count == 0) {
            std::cout << '\x07';
        } else if (context.tabSM->count > 0 && (allCmds.size() > 1 || context.path->search(extCmd))) { // TODO: Check the logic here
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
        }
    } else {
        std::string str = remove_start(extCmd, cmd);
        for(auto it : str) {
            cmd.push_back(it);
            std::cout << it;
        }
        cmd.push_back(' ');
        std::cout << ' ';
    }
}

void handleAutocompetePath(std::string &cmdFull, ShellContext &context) {
    std::istringstream iss(cmdFull);
    std::vector<std::string> args;
    std::string last, part, parts;
    fs::path cwd, finalDir;
    while(iss >> last) {
        args.push_back(last);
    }

    for(auto it : last) {
        if(it == '~') continue;
        part += it;
        if(it == '/') {
            parts = parts.length() > 0 ? parts + '/' + part : parts + part;
            part = "";
            continue;
        }
    }
    
    if(!last.empty() && last[0] != '~') {
        cwd = fs::current_path();
    } else if (last[0] == '~') {
        const char* home = std::getenv("USERPROFILE");;
        cwd = home;
    }

    finalDir = cwd / parts;
    populate_dir(context.newDir, finalDir);
    
    std::string cmd = part;
    std::string extCmd = context.newDir->extendPrefix(cmd);
    std::vector<std::string> allCmds = context.newDir->allCmdsFromPrefix(cmd);
    if((extCmd == cmd && !(context.newDir->search(extCmd))) || (allCmds.size() > 1)) {
        if(allCmds.size() == 0 && context.tabSM->count == 0) {
            std::cout << '\x07';
        } else if (context.tabSM->count > 0 && (allCmds.size() > 1 || context.newDir->search(extCmd))) { // TODO: Check the logic here
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
            cmdFull += diff;
            std::cout << cmdFull;
            std::vector<std::string> currArgs = context.newDir->getInfo(extCmd);
            if(context.tabSM->count == 2 && !currArgs.empty() && currArgs[0] == "directory") {
                cmdFull += '/';
                std::cout << '/';
            } 
        }
    } else {
        std::string str = remove_start(extCmd, cmd);
        for(auto it : str) {
            cmdFull.push_back(it);
            std::cout << it;
        }
        std::vector<std::string> currArgs = context.newDir->getInfo(extCmd);
        if(!currArgs.empty() && currArgs[0] == "directory") {
            cmdFull += '/';
            std::cout << '/';
        }
    }
}

void shell_read(std::string &line, int &status, ShellContext &context) { // TODO: polish waiting for second tab logic
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

        if (!has_space(inputBuffer)) { // TODO: Create seperate function for this
            if(context.tabSM->count < 3) context.tabSM->curr = TAB_AUTOCOMP_CMD;
            else context.tabSM->curr = TAB_AUTOCOMP_NONE;
        } else {
            if(context.tabSM->count < 3 || context.tabSM->pathsPossible) context.tabSM->curr = TAB_AUTOCOMP_PATH;
            else context.tabSM->curr = TAB_AUTOCOMP_NONE;
        }

        if(ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
            auto &ke = ir.Event.KeyEvent;
            WORD vk = ke.wVirtualKeyCode;
            char ch = ke.uChar.AsciiChar;

            if(vk != VK_TAB) {
                context.tabSM->count = 0;
            }

            if(vk == VK_ESCAPE) {
                if(context.tabSM->count > 0) {
                    context.newDir->clear();
                    context.newDir = new Trie(*context.CurrDir);
                }
                std::cout << '\n';
                break;
            }

            if(vk == VK_BACK) {
                if(!inputBuffer.empty()) {
                    inputBuffer.pop_back();
                    std::cout<< "\b \b" << std::flush;
                }
            } else if (vk == VK_TAB) { // TODO: Add support for two tabs
                if (context.tabSM->curr == TAB_AUTOCOMP_CMD) {
                    handleAutocompeteCmd(inputBuffer, context);
                } else if (context.tabSM->curr == TAB_AUTOCOMP_PATH) {
                    handleAutocompetePath(inputBuffer, context);
                } else {
                    int spacesToAdd = TAB_SIZE - (static_cast<int>(inputBuffer.size()) % TAB_SIZE);
                    for (int i = 0; i < spacesToAdd; ++i) {
                        inputBuffer.push_back(' ');
                        std::cout << ' ';
                    }
                }
                context.tabSM->count++;
                std::cout << std::flush;
            } else if (vk == VK_RETURN) {
                if(context.tabSM->count > 0) {
                    context.newDir->clear();
                    context.newDir = new Trie(*context.CurrDir);
                }
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
