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
#include <algorithm>
#include "shell_context.hpp"
#include "utils.hpp"

std::string longestCommonPrefix(std::vector<std::string>& cmdList, std::string& cmd);
void handle_autocomplete(std::string &cmd, ShellContext &context);

void shell_read(std::string &line, int &status, ShellContext &context);