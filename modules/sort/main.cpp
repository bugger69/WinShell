#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "context.hpp"

int extractArgsAndFlags(std::vector<std::string> &args, std::vector<std::string> &arg, std::vector<std::string> &flags) {
    for(auto it : args) {
        if(it.starts_with('-')) {
            flags.push_back(it);
        } else {
            arg.push_back(it);
        }
    }
    return 0;
}

int sortedInsert(std::istream &strm, std::vector<std::string> &output) {
    return 0;
}

int sortFile(std::vector<std::string> &arg, std::vector<std::string> &flags) {
    fs::path filepath(arg[0]);
    std::ifstream file(filepath);
    std::vector<std::string> sortedFile;

    if(sortedInsert(file, sortedFile)) {
        std::cerr << "sort: unable to sort" << std::endl;
    }

    for(auto it : sortedFile) {
        std::cout << it << std::endl;
    }

    return 0;
}

int sortStream(std::vector<std::string> &arg, std::vector<std::string> &flags) {
    return 0;
}

int main (int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> arg, flags;
    int ret = 0;

    ret = extractArgsAndFlags(args, arg, flags);
    
    if(arg.size() >= 1) {
        if(ret == 0) ret = sortFile(arg, flags);
    } else {
        if(ret == 0) ret = sortStream(arg, flags); 
    }

    return ret;
}