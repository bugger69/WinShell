#include <iostream>
#include <vector>
#include <regex>

void read_from_stdout(std::vector<std::string> args) {}

void read_from_file(std::vector<std::string> args) {}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);

    args.size() > 1 ? read_from_file(args) : read_from_stdout(args);

    return 0;
}