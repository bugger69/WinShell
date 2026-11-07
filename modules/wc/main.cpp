#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include "context.hpp"

void setMode(std::vector<std::string> &flags, Params &par) {
    for(auto it : flags) {
        if(it == "-c") {
            par.mode = WC_MODE_SIZE;
        } else if (it == "-l") {
            par.mode = WC_MODE_LINES;
        } else if (it == "-w") {
            par.mode = WC_MODE_WORDS;
        } else if (it == "-m") {
            par.mode = WC_MODE_CHARS;
        }
    }
}

int wcFromStream(std::ifstream &fstrm, Params &par) {
    if(par.mode == WC_MODE_SIZE) {
        std::streamsize size = fstrm.tellg();
        fstrm.seekg(0, std::ios::beg);
        std::cout << static_cast<int>(size); 
    } else if (par.mode == WC_MODE_LINES) {
        std::string line;
        int count = 0;

        fstrm.seekg(0, std::ios::beg);
        while(getline(fstrm, line)) {
            count++;
        }
        std::cout << count;
    } else if (par.mode == WC_MODE_WORDS) {
        std::string word;
        int count = 0;

        fstrm.seekg(0, std::ios::beg);
        while(fstrm >> word) {
            count++;
        }
        std::cout << count;
    } else if (par.mode == WC_MODE_CHARS) {
        char ch;
        int count = 0;

        fstrm.seekg(0, std::ios::beg);
        while(fstrm.get(ch)) {
            count++;
        }
        std::cout << count;
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