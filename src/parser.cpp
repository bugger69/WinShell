#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "utils.hpp"
#include "shell_context.hpp"

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
        } else if (i > 0 && (args[i - 1] == ">>" || args[i - 1] == "1>>") && (args[i] != ">>" || args[i] != "1>>")) {
            consoleOut.flag_std = SHELL_CMD_OUT_STD_APPEND;
            consoleOut.file_std = args[i];
            toErase.push_back(i - 1);
            toErase.push_back(i);
        } else if (i > 0 && (args[i - 1] == ">>" || args[i - 1] == "1>>") && (args[i] == ">>" || args[i] == "1>>")) {
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

void shell_parse(const std::string &line, std::vector<std::string> &args, ShellContext &context) { // TODO: improve error handling
    int i = 0;
    std::string curr;
    context.consoleOut = new OutputTarget();
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
        if(!isEscape(line, i) && (line[i] == ' ' || line[i] == ';' || line[i] == '|')) {
            if(!curr.empty()) args.push_back(curr);
            if(line[i] == ';' || line[i] == '|') args.push_back({line[i]});
            curr.clear();
            i++;
            continue;
        }
        if(!isCurrEscape(line, i)) curr += line[i];
        i++;
    }

    if(!curr.empty()) {
        std::string rplStr = replaceShellVariables(curr);
        args.push_back(rplStr);
    }
    
    curr.clear();
    if (context.consoleOut != nullptr) {
        setConsoleOutputBuf(args, *context.consoleOut);
    }
}