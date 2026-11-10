#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "prefixTree.hpp"

namespace fs = std::filesystem;

/* Encoder Helpers */
std::vector<std::uint8_t> packBits(const std::string& bits) {
    std::vector<std::uint8_t> bytes;
    std::uint8_t byte = 0;
    int bitCount = 0;

    for (char bit : bits) {
        if (bit != '0' && bit != '1') {
            throw std::invalid_argument("Bit string must contain only '0' and '1'");
        }

        byte = static_cast<std::uint8_t>((byte << 1) | (bit == '1' ? 1 : 0));
        if (++bitCount == 8) {
            bytes.push_back(byte);
            byte = 0;
            bitCount = 0;
        }
    }

    return bytes;
}

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

int populatePrefix(PrefixTree &tree, std::map<char, std::vector<std::uint8_t>> &prefixCodes, std::map<char, int> &frequencies) {
    for(auto it : frequencies) {
        prefixCodes[it.first] = packBits(tree.getPrefixCode(it.first));
    }
    return 0;
}

int generateEncodedFile(fs::path &outfile, std::ifstream &file, std::map<char, std::vector<std::uint8_t>> &prefixCodes) {
    char ch;
    std::ofstream out(outfile, std::ios::binary);
    file.seekg(0, std::ios::beg);

    while(file.get(ch)) {

    }
    return 0;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::map<char, int> frequencies;
    std::map<char, std::vector<std::uint8_t>> prefixCodes;
    fs::path filepath(args[0]);
    fs::path outpath(args[0]);
    std::ifstream file(filepath, std::ios::binary);
    std::vector<Node> nodes;
    PrefixTree tree;
    int ret = 0;

    if(!file.is_open()) {
        std::cout << "compress: file " << filepath << " not found." << std::endl;
        ret = -1;
    }

    if(args.size() == 1 || (args.size() > 1 && args[1] == "encode")) {
        if(ret == 0) ret = populateFrequencies(file, frequencies);

        if(ret == 0) ret = populateNodes(frequencies, nodes);

        tree.insert(nodes);
    
        if(ret == 0) ret = populatePrefix(tree, prefixCodes, frequencies);
        outpath += ".cmpr";

        if(ret = 0) ret = generateEncodedFile(outpath, file, prefixCodes);
    }

    return ret;
}