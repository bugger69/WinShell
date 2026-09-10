#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include "utils.hpp"

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

void prefill_input(const std::string& text) { 
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    if (hStdin == INVALID_HANDLE_VALUE) return; // Each character requires a 'key down' and a 'key up' event 
    std::vector<INPUT_RECORD> events; 
    for (char c : text) { 
        INPUT_RECORD ir = {}; 
        ir.EventType = KEY_EVENT; 
        ir.Event.KeyEvent.bKeyDown = TRUE; 
        ir.Event.KeyEvent.wRepeatCount = 1; 
        ir.Event.KeyEvent.uChar.AsciiChar = c; 
        events.push_back(ir); 
    } 
    DWORD written; 
    WriteConsoleInput(hStdin, events.data(), events.size(), &written); 
}