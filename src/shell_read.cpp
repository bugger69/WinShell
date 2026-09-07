#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <windows.h>
#include <algorithm>
#include "shell_read.hpp"
#include "utils.hpp"

void shell_read(std::string &line) {
    std::getline(std::cin, line);
}
