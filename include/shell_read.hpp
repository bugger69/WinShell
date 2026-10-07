#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <conio.h>
#include <map>
#include "shell_context.hpp"
#include "utils.hpp"

struct line_state {
    std::size_t cursPos;
    std::string inputBuf;
};

void setAutoCompState(std::string &buf, ShellContext &context);

void handleAutocompetePath(line_state *inputSM, ShellContext &context);
void handleAutocompeteCmd(line_state *inputSM, ShellContext &context);
void handleAutocompeteComp(line_state *inputSM, ShellContext &context);

void shell_read(std::string &line, int &status, ShellContext &context);