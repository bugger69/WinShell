#include <iostream>
#include <vector>
#include <string>

int extractArgsAndFlags(std::vector<std::string> &args, std::vector<std::string> &arg, std::vector<std::string> &flags) {
    
    return 0;
}

int main (int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> arg, flags;
    int ret = 0;

    ret = extractArgsAndFlags(args, arg, flags);

    return ret;
}