#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "context.hpp"

void setMode(std::vector<std::string> &flags, Params &par) {
    for(auto it : flags) {
        if(it == "-c") {
            par.mode = WC_MODE_SIZE;
        }
    }
}

int wcFromStream(std::ifstream &fstrm, Params &par) {
    if(par.mode == WC_MODE_SIZE) {
        std::streamsize size = fstrm.tellg();
        fstrm.seekg(0, std::ios::beg);
        std::cout << static_cast<int>(size); 
    }
    return 0;
}

int readFromFile(std::vector<std::string> &arg, Params &par) {
    fs::path filePath(arg[0]);
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    int ret;
    if(!file.is_open()) {
        std::cerr << "Unable to open file." << std::endl;
        return -1;
    }
    ret = wcFromStream(file, par);
    if(ret != -1) std::cout << " " << filePath.string() << std::endl;
    return ret;
}

int readFromStream(std::vector<std::string> &arg, Params &par) {
    return 0;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> arg, flags;
    Params par;
    int ret = -1;
    for(auto it : args) {
        if(!it.empty() && it.front() == '-') {
            flags.push_back(it);
        } else {
            arg.push_back(it);
        }
    }

    setMode(flags, par);

    ret = arg.size() > 0 ? readFromFile(arg, par) : readFromStream(arg, par);

    return ret;
}