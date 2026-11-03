#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <filesystem>
#include "context.hpp"

namespace fs = std::filesystem;

void setParamsFromFlags(std::vector<std::string> &flags, Params &par) {
    for(auto it : flags) {
        if(it == "-o") {
            par.onlyMatching = true;
        }
    }
}

int grep_from_stream(std::istream& in, std::string pattern, Params &par) {
    std::string line;
    std::smatch matches;
    std::regex re(pattern);
    bool didMatch = false;
    while(getline(in, line)) {
        if(par.onlyMatching) {
            for(std::sregex_iterator it(line.begin(), line.end(), re), end; it != end; ++it) {
                std::cout << it->str() << '\n';
                didMatch = true;
            }
        } else {
            if(std::regex_search(line, matches, re)) {
                std::cout << line << '\n';
                didMatch = true;
            }
        }
    }
    return !didMatch;
}

int read_from_stdout(std::vector<std::string> args, Params &par) {
    std::string pattern(args[args.size() - 1]);

    return grep_from_stream(std::cin, pattern, par);
}

int read_from_file(std::vector<std::string> args, Params &par) {
    std::string pattern(args[args.size() - 2  < 0 ? 0 : args.size() - 2]);
    fs::path filepath(args[args.size() - 1]);
    std::ifstream fstr(filepath);

    return grep_from_stream(fstr, pattern, par);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> fullArgs(argv + 1, argv + argc);
    std::vector<std::string> args, flags;
    Params par;

    for(auto it : fullArgs) {
        if(!it.empty() && it[0] != '-') {
            args.push_back(it);
        } else flags.push_back(it);
    }

    setParamsFromFlags(flags, par);

    return args.size() > 1 ? read_from_file(args, par) : read_from_stdout(args, par);
}