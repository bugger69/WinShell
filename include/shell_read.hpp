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

void handleAutocompetePath(std::string &arg, ShellContext &context);
void handleAutocompeteCmd(std::string &cmd, ShellContext &context);

void shell_read(std::string &line, int &status, ShellContext &context);