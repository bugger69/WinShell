#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void get_from_stream(std::istream& in, std::string pattern) {
    std::string line;
    while(getline(in, line)) {
        std::cout << line << '\n';
    }
}

void read_from_stdout(std::vector<std::string> args) {
    std::string pattern(args[0]);

    get_from_stream(std::cin, pattern);
}

void read_from_file(std::vector<std::string> args) {
    std::string pattern(args[0]);
    fs::path filepath(args[1]);
    std::ifstream fstr(filepath);

    get_from_stream(fstr, pattern);
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    args.size() > 1 ? read_from_file(args) : read_from_stdout(args);

    return 0;
}