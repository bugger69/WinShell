#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "context.hpp"

bool toChar(const std::string& text, char& result) {
    if (text.size() == 1) {
        result = text[0];
        return true;
    }

    if (text == "\\t") {
        result = '\t';
        return true;
    }
    if (text == "\\n") {
        result = '\n';
        return true;
    }

    return false;
}

int populateFlagOpts(std::vector<std::string> &args, std::vector<std::string> &flags, std::string &file, FlagData &flgdata) {
    for(auto it : args) {
        if(!it.empty() && it.front() == '-') {
            if(it.starts_with("-d")) {
                std::string prefix = it.substr(0, 2);
                std::string del = it.substr(2, it.size());
                char delimiter;
                if(toChar(del, delimiter)) {
                    flgdata.delimiter = delimiter;
                } else {
                    std::cerr << "cut: invalid delimiter" << std::endl;
                }
            } else flags.push_back(it);
        }
    }
    if(!args.empty() && (args.back().empty() || args.back().front() != '-')) file = args.back();
    return 0;
}

int dumpKthField(std::ifstream &fstrm, int k, char delimiter = '\t') {
    std::string line;
    while(getline(fstrm, line)) {
        std::string word;
        std::stringstream ln(line);
        int i = 0;
        while(getline(ln, word, delimiter)) {
            if(i == k) std::cout << word;
            i++;
        }
        std::cout << std::endl;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> flags;
    std::string filename;
    FlagData flgdata;
    int ret = 0;

    ret = populateFlagOpts(args, flags, filename, flgdata);
    if(ret != 0 || flags.empty()) {
        std::cerr << "cut: flags needed" << std::endl;
        ret = -1;
    }

    if(!filename.empty()) {
        std::ifstream file(filename);
        if(ret == 0 && !file.is_open()) {
            std::cerr << "cut: Unable to open file" << std::endl;
            ret = -1;
        }
        if(ret == 0) ret = dumpKthField(file, 2, flgdata.delimiter);
    }
    return ret;
}