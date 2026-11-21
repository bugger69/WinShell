#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "context.hpp"

int setFlagOpts(std::vector<std::string> &flags, FlagOptions &opts) {
    for(auto it : flags) {
        if(it == "-u") {
            opts.unique = true;
        } else if (it.starts_with("--sort=")) {
            std::string pre = it.substr(0, 6);
            std::string sort = it.substr(7, it.size());
            if(sort == "insertion") {
                opts.sortType = SORT_TYPE_INSERT;
            } else if (sort == "merge") {
                opts.sortType = SORT_TYPE_MERGE;
            } else {
                std::cerr << "sort: Invalid sort type" << std::endl;
                return -1;
            }
        }
    }
    return 0;
}

int extractArgsAndFlags(std::vector<std::string> &args, std::vector<std::string> &arg, std::vector<std::string> &flags) {
    for(auto it : args) {
        if(it.starts_with('-')) {
            flags.push_back(it);
        } else {
            arg.push_back(it);
        }
    }
    return 0;
}

int sortedInsert(std::vector<std::string> &output, std::string &line) {
    for(std::size_t i = 0; i < output.size(); i++) {
        if(line < output[i]) {
            output.insert(output.begin() + i, line);
            return 0;
        }
    }
    output.push_back(line);
    return 0;
}

int sortedMerge(std::vector<std::string> &output, int left, int mid, int right) {
    return 0;
}

int mergeSort(std::vector<std::string> &output) {

    return 0;
}

int sortWithType(std::istream &strm, std::vector<std::string> &output, FlagOptions &opts) {
    std::string line;
    while(getline(strm, line)) {
        if(opts.sortType == SORT_TYPE_INSERT) {
            if(sortedInsert(output, line)) {
                std::cerr << "sort: unable to insert" << std::endl;
                return -1;
            } else output.push_back(line);
        }
    }

    if(opts.sortType == SORT_TYPE_DEF) {
        sort(output.begin(), output.end());
    } else if (opts.sortType == SORT_TYPE_MERGE) {
        mergeSort(output);
    }

    return 0;
}

int sortFile(std::vector<std::string> &arg, FlagOptions &opts) {
    fs::path filepath(arg[0]);
    std::ifstream file(filepath);
    std::vector<std::string> sortedFile;
    std::string prev;
    bool hasPrev = false;

    if(sortWithType(file, sortedFile, opts)) {
        std::cerr << "sort: unable to sort" << std::endl;
    }

    for(auto it : sortedFile) {
        if(opts.unique) {
            if(!hasPrev || it != prev) {
                std::cout << it << std::endl;
            }
            prev = it;
            hasPrev = true;
        } else {
            std::cout << it << std::endl;
        }
    }

    return 0;
}

int sortStream(std::vector<std::string> &arg, FlagOptions &opts) {
    return 0;
}

int main (int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> arg, flags;
    FlagOptions opts;
    int ret = 0;

    ret = extractArgsAndFlags(args, arg, flags);
    
    ret = setFlagOpts(flags, opts);

    if(arg.size() >= 1) {
        if(ret == 0) ret = sortFile(arg, opts);
    } else {
        if(ret == 0) ret = sortStream(arg, opts); 
    }

    return ret;
}