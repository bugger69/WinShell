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

void setAutoCompState(std::string &buf, ShellContext &context) {
        if (!has_space(buf)) { // TODO: Create seperate function for this
            if(context.tabSM->count < 3) context.tabSM->curr = TAB_AUTOCOMP_CMD;
            else context.tabSM->curr = TAB_AUTOCOMP_NONE;
        } else {
            std::string cmd;
            for(auto it : buf) {
                if(it == ' ') break;
                cmd.push_back(it);
            }
            if(context.cmdComp.find(cmd) != context.cmdComp.end() && context.cmdComp[cmd]->compFlag != COMPLETE_AUTOCOMPLETE_FILE && context.cmdComp[cmd]->compFlag != COMPLETE_AUTOCOMPLETE_DIRECTORY){ 
                context.tabSM->curr = TAB_AUTOCOMP_ARG;
            } else if(context.tabSM->count < 3) context.tabSM->curr = TAB_AUTOCOMP_PATH;
            else context.tabSM->curr = TAB_AUTOCOMP_NONE;
        }
}

void handleAutocompeteCmd(line_state *inputSM, ShellContext &context) {
    std::string extCmd = context.path->extendPrefix(inputSM->inputBuf);
    std::vector<std::string> allCmds = context.path->allCmdsFromPrefix(inputSM->inputBuf);
    if((extCmd == inputSM->inputBuf && !(context.path->search(extCmd))) || (allCmds.size() > 1)) {
        if(allCmds.size() == 0 && context.tabSM->count == 0) {
            std::cout << '\x07';
        } else if (context.tabSM->count > 0 && (allCmds.size() > 1 || context.path->search(extCmd))) { // TODO: Check the logic here
            std::string diff = remove_start(extCmd, inputSM->inputBuf);
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
            inputSM->inputBuf += diff;
        }
    } else {
        std::string str = remove_start(extCmd, inputSM->inputBuf);
        for(auto it : str) {
            inputSM->inputBuf.push_back(it);
            std::cout << it;
        }
        inputSM->inputBuf.push_back(' ');
        std::cout << ' ';
    }
}

void handleAutocompetePath(line_state *inputSM, ShellContext &context) {
    std::istringstream iss(inputSM->inputBuf);
    std::vector<std::string> args;
    std::string maincmd, last, part, parts;
    fs::path cwd, finalDir;

    for(auto it : inputSM->inputBuf) {
        if(it == ' ') break;
        maincmd += it;
    }

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
    std::string extCmd;
    bool onlyDirs = false;
    std::vector<std::string> allCmds;

    if(context.cmdComp.find(maincmd) != context.cmdComp.end() && context.cmdComp[maincmd]->compFlag == COMPLETE_AUTOCOMPLETE_DIRECTORY) {
        onlyDirs = true;
    } 
    if(onlyDirs) { 
        extCmd = context.newDir->extendPrefix(cmd, "directory");
        allCmds = context.newDir->allCmdsFromPrefix(cmd, "directory");
    } else {
        extCmd = context.newDir->extendPrefix(cmd);
        allCmds = context.newDir->allCmdsFromPrefix(cmd);
    }

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
                bool isdir = false;
                std::vector<std::string> info = context.newDir->getInfo(allCmds[i]);
                for(auto it : info) {
                    if(it == "directory") isdir = true;
                }

                if(isdir) {
                    std::cout << std::left << std::setw(static_cast<int>(columnWidth))
                        << allCmds[i] + '/';
                } else {
                    std::cout << std::left << std::setw(static_cast<int>(columnWidth))
                        << allCmds[i];
                }

                if ((i + 1) % 3 == 0) {
                    std::cout << '\n';
                }
            }

            if (allCmds.size() % 3 != 0) {
                std::cout << '\n';
            }
            std::cout << '>';
            std::cout << ' ';
            inputSM->inputBuf += diff;
            std::cout << inputSM->inputBuf;
            std::vector<std::string> currArgs = context.newDir->getInfo(extCmd);
            if(context.tabSM->count == 2 && !currArgs.empty() && currArgs[0] == "directory") {
                inputSM->inputBuf += '/';
                std::cout << '/';
            }
        }
    } else {
        std::string str = remove_start(extCmd, cmd);
        for(auto it : str) {
            inputSM->inputBuf.push_back(it);
            std::cout << it;
        }
        std::vector<std::string> currArgs = context.newDir->getInfo(extCmd);
        if(!currArgs.empty() && currArgs[0] == "directory") {
            inputSM->inputBuf += '/';
            std::cout << '/';
        } else {
            inputSM->inputBuf += ' ';
            std::cout << ' ';
        }
    }
}

void handleAutocompeteComp(line_state *inputSM, ShellContext &context) { // TODO: Fix Bash Handling, esp path existing case.
    std::string cmd;
    std::string part;
    std::istringstream iss(inputSM->inputBuf);
    std::vector<std::string> args;
    for(auto it : inputSM->inputBuf) {
        if(it == ' ') break;
        cmd += it;
    }

    while(iss >> part) {
        args.push_back(part);
    }

    try {
        if(context.cmdComp[cmd]->compFlag == COMPLETE_AUTOCOMPLETE_SCRIPT) {
            ProcessResult Out;
            fs::path currd = fs::current_path();
            std::string bash = "C:/Program Files/Git/bin/bash.exe";
            std::string cwd = currd.string();
            std::string completer = context.cmdComp[cmd]->execPath;
            std::wstring cmdStr;
            std::wstring compLine(inputSM->inputBuf.begin(), inputSM->inputBuf.end());
            std::wstring compPoint = std::to_wstring(inputSM->cursPos);
            cwd += '/';
            
            if(fs::is_regular_file(bash) && endsWith(completer, ".sh")) cmdStr += std::wstring(bash.begin(), bash.end()) + L" ";
            cmdStr += std::wstring(cwd.begin(), cwd.end());
            cmdStr += std::wstring(completer.begin(), completer.end()) + L" ";
            cmdStr += std::wstring(cmd.begin(), cmd.end()) + L" ";
            cmdStr += std::wstring(part.begin(), part.end()) + L" ";
            if(args.size() > 2) {
                std::string prev = args[args.size() - 2];
                cmdStr += std::wstring(prev.begin(), prev.end()) + L" ";
            } else {
                cmdStr += L"\"\"";
            }
            SetEnvironmentVariableW(L"COMP_LINE", compLine.c_str());
            SetEnvironmentVariableW(L"COMP_POINT", compPoint.c_str());
            try {
                Out = getOutputFromProcess(cmdStr);
            } catch (...) {
                SetEnvironmentVariableW(L"COMP_LINE", nullptr);
                SetEnvironmentVariableW(L"COMP_POINT", nullptr);
                throw;
            }
            SetEnvironmentVariableW(L"COMP_LINE", nullptr);
            SetEnvironmentVariableW(L"COMP_POINT", nullptr);
            if(Out.exitCode) {
                throw std::runtime_error("Unable to run compiler Script: output:" + Out.output + ": ");
            }
            if(!Out.output.empty()) {
                std::vector<std::string> out = splitWords(Out.output);
                if(out.size() == 0) {
                    std::cout << '\x07';
                } else if (out.size() == 1) {
                    std::string finalCmd = out[0];
                    std::string diff = remove_start(finalCmd, part);
                    inputSM->inputBuf += diff;
                    inputSM->inputBuf += ' ';
                    std::cout << diff;
                    std::cout << ' ';
                } else {
                    std::cout << '\n';
                    for(auto it : out) {
                        std::cout << it;
                        std::cout << std::endl;
                    }
                    std::cout << '>';
                    std::cout << ' ';
                    std::cout << inputSM->inputBuf;
                }

            } else {
                std::cout << '\x07';
            }

        }
    } catch (std::runtime_error e) {
        std::cout << "runtime error: " << e.what() << std::endl;
    }
}

void shell_read(std::string &line, int &status, ShellContext &context) { // TODO: polish waiting for second tab logic
    line_state *inputSM = new line_state();
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD prevHandle;
    GetConsoleMode(hIn, &prevHandle);

    SetConsoleMode(hIn, prevHandle & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

    INPUT_RECORD ir;
    DWORD read;
    bool waitingForSecondTab = false;

    while(true) {
        ReadConsoleInput(hIn, &ir, 1, &read);

        setAutoCompState(inputSM->inputBuf, context);

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
                if(!inputSM->inputBuf.empty()) {
                    inputSM->inputBuf.pop_back();
                    std::cout<< "\b \b" << std::flush;
                }
            } else if (vk == VK_TAB) {
                if (context.tabSM->curr == TAB_AUTOCOMP_ARG) {
                    handleAutocompeteComp(inputSM, context);
                } else if (context.tabSM->curr == TAB_AUTOCOMP_CMD) {
                    handleAutocompeteCmd(inputSM, context);
                } else if (context.tabSM->curr == TAB_AUTOCOMP_PATH) {
                    handleAutocompetePath(inputSM, context);
                } else {
                    int spacesToAdd = TAB_SIZE - (static_cast<int>(inputSM->inputBuf.size()) % TAB_SIZE);
                    for (int i = 0; i < spacesToAdd; ++i) {
                        inputSM->inputBuf.push_back(' ');
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
                line = inputSM->inputBuf;
                std::cout << std::endl;
                inputSM->inputBuf.clear();
                break;
            } else if (ch >= 32 && ch <= 126) {
                inputSM->inputBuf.push_back(ch);
                std::cout<<ch<<std::flush;
            }
        }
        inputSM->cursPos = inputSM->inputBuf.size();
    }
    SetConsoleMode(hIn, prevHandle);
    delete inputSM;
}
