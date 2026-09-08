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
    char ch;
    while(std::cin.get(ch) && ch != '\n') {
        line += ch;
    }
}
