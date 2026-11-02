#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int grep_from_stream(std::istream& in, std::string pattern) {
    std::string line;
    std::smatch matches;
    std::regex re(pattern);
    while(getline(in, line)) {
        if(std::regex_search(line, matches, re)) {
            std::cout << line << '\n';
            return 0;
        }
    }
    return 1;
}

int read_from_stdout(std::vector<std::string> args) {
    std::string pattern(args[args.size() - 1]);

    return grep_from_stream(std::cin, pattern);
}

int read_from_file(std::vector<std::string> args) {
    std::string pattern(args[args.size() - 2  < 0 ? 0 : args.size() - 2]);
    fs::path filepath(args[args.size() - 1]);
    std::ifstream fstr(filepath);

    return grep_from_stream(fstr, pattern);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    return args.size() > 1 ? read_from_file(args) : read_from_stdout(args);
}