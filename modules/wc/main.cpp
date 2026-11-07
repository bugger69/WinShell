#include <iostream>
#include <vector>
#include "context.hpp"

void setMode(std::vector<std::string> &flags, Params &par) {
    for(auto it : flags) {
        if(it == "-c") {
            par.mode = WC_MODE_SIZE;
        }
    }
}

int readFromFile(std::vector<std::string> &arg, Params &par) {
    return 0;
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