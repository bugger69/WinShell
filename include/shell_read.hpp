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
#include "utils.hpp"

void handle_autocomplete(std::string &cmd);

void shell_read(std::string &line, int &status);