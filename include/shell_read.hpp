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

void handle_autocomplete(std::string &cmd, ShellContext &context, bool &waitingForSecondTab);

void shell_read(std::string &line, int &status, ShellContext &context);