#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include "prefixTree.hpp"

namespace fs = std::filesystem;

int populateFrequencies(std::ifstream& file, std::map<char, int>& frequencies) {
    char ch;
    while(file.get(ch)) {
        frequencies[ch]++;
    }
    return 0;
}

int populateNodes(std::map<char, int>& frequencies, std::vector<Node>& nodes) {
    for(auto it : frequencies) {
        int val = it.first;
        int freq = it.second;
        Node* n = new Node(val, '\0', freq);
        nodes.push_back(*n);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::map<char, int> frequencies;
    fs::path filepath(args[0]);
    std::ifstream file(filepath);
    std::vector<Node> nodes;
    PrefixTree tree;
    int ret = 0;

    if(!file.is_open()) {
        std::cout << "compress: file " << filepath << " not found." << std::endl;
        ret = -1;
    }

    if(ret == 0) ret = populateFrequencies(file, frequencies);

    if(ret == 0) ret = populateNodes(frequencies, nodes);

    tree.insert(nodes);

    return ret;
}