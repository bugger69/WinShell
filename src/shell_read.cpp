#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <limits>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <algorithm>
#include "shell_cmds.hpp"
#include "shell_read.hpp"
#include "utils.hpp"

void handle_autocomplete(std::string &line) {
    if(line == "exi") {
        prefill_input("exit");
        // std::cout<<"exit"<<std::endl;
    } else if (line == "ech") {
        prefill_input("echo");
        // std::cout<<"echo"<<std::endl;
    }
}

void shell_read(std::string &line, int &status) {
    char ch;
    while(std::cin.get(ch) && (ch != '\n')) {
        if(ch == '\t') {
            status = SHELL_STATUS_NORM; // TODO: turn this to an extra command here
            handle_autocomplete(line);
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );
            std::string var = "autocomp " + line;
            line = var;
            return;
        } else {
            line += ch;
        }
    }
}
