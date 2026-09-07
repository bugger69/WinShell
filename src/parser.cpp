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

void shell_parse(const std::string &line, std::vector<std::string> &args) { // TODO: improve error handling
    int i = 0;
    std::string curr;
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
        if(!isEscape(line, i) && line[i] == ' ') {
            if(!curr.empty()) args.push_back(curr);
            curr = "";
            i++;
            continue;
        }
        if(!isCurrEscape(line, i)) curr += line[i];
        i++;
    }
    if(!curr.empty()) args.push_back(curr);
    curr = "";
}