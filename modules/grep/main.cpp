#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <io.h>
#include "context.hpp"

namespace fs = std::filesystem;

void setParamsFromFlags(std::vector<std::string> &flags, Params &par) {
    for(auto it : flags) {
        if(it == "-o") {
            par.onlyMatching = true;
        } else if (it.compare(0, 8, "--color=") == 0) {
            std::string mode = "";
            for(size_t i = 0; i < it.size(); i++) {
                if(i > 7) {
                    mode += it[i];
                }
            }
            if(mode == "none") {
                par.highlight = HIGHLIGHT_NONE;
            } else if (mode == "auto") {
                par.highlight = HIGHLIGHT_AUTO;
            } else if (mode == "always") {
                par.highlight = HIGHLIGHT_ALWAYS;
            }
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
            if(par.highlight == HIGHLIGHT_NONE) {
                if(std::regex_search(line, matches, re)) {
                    std::cout << line << '\n';
                    didMatch = true;
                }
            } else {
                std::size_t cursor = 0;
                bool lineMatched = false;
                const bool useColor = par.highlight == HIGHLIGHT_ALWAYS || (par.highlight == HIGHLIGHT_AUTO && _isatty(_fileno(stdout)));

                for (std::sregex_iterator it(line.begin(), line.end(), re), end;
                    it != end;
                    ++it) {
                    const auto start = static_cast<std::size_t>(it->position());
                    const auto length = static_cast<std::size_t>(it->length());
                    
                    if(useColor) {
                        std::cout << line.substr(cursor, start - cursor)
                            << "\033[1;31m"
                            << it->str()
                            << "\033[0m";
                    } else {
                        std::cout << line.substr(cursor, start - cursor)
                            << it->str();
                    }          

                    cursor = start + length;
                    lineMatched = true;
                }

                if (lineMatched) {
                    std::cout << line.substr(cursor) << '\n';
                    didMatch = true;
                }
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

int read_from_files(std::vector<std::string> args, Params &par) {
    std::string pattern(args[0]);
    std::vector<std::string> arg;
    bool didget = false;
    for(int i = 0; i < args.size(); i++) {
        if(i > 0) {
            fs::path filepath(args[i]);
            arg.push_back(args[i]);
            if(fs::exists(filepath)) {
                didget += read_from_file(arg, par);
            } else {
                std::cerr << "Unable to find file: " << filepath.string() << std::endl;
            }
            arg.pop_back();
        } else {
            arg.push_back(args[i]);
        }
    }
    return didget;
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

    return args.size() > 1 ? read_from_files(args, par) : read_from_stdout(args, par);
}