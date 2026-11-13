#include <iostream>
#include <vector>

int populateFlagOpts(std::vector<std::string> &args, std::vector<std::string> &flags, std::string &file) {
    for(auto it : args) {
        if(!it.empty() && it.front() == '-') {
            flags.push_back(it);
        }
    }
    if(!args.empty() && (args.back().empty() || args.back().front() != '-')) file = args.back();
    return 0;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> flags;
    std::string file;
    int ret = 0;

    ret = populateFlagOpts(args, flags, file);
    if(flags.empty()) {
        std::cerr << "cut: flags needed" << std::endl;
        ret = -1;
    }
    return ret;
}