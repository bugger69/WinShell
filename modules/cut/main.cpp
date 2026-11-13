#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

int populateFlagOpts(std::vector<std::string> &args, std::vector<std::string> &flags, std::string &file) {
    for(auto it : args) {
        if(!it.empty() && it.front() == '-') {
            flags.push_back(it);
        }
    }
    if(!args.empty() && (args.back().empty() || args.back().front() != '-')) file = args.back();
    return 0;
}

int dumpKthField(std::ifstream &fstrm, int k, char delimiter = ' ') {
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
    int ret = 0;

    ret = populateFlagOpts(args, flags, filename);
    if(flags.empty()) {
        std::cerr << "cut: flags needed" << std::endl;
        ret = -1;
    }

    if(!filename.empty()) {
        std::ifstream file(filename);
        if(ret == 0 && !file.is_open()) {
            std::cerr << "cut: Unable to open file" << std::endl;
            ret = -1;
        }
        if(ret == 0) ret = dumpKthField(file, 2, '\t');
    }
    return ret;
}