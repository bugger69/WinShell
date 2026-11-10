#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "prefixTree.hpp"

namespace fs = std::filesystem;

/* Encoder Helpers */
struct prefix {
    int length;
    std::vector<std::uint8_t> prefixCode;
};

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

    if (bitCount > 0) {
        bytes.push_back(static_cast<std::uint8_t>(byte << (8 - bitCount)));
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

int populatePrefix(PrefixTree &tree, std::map<char, prefix> &prefixCodes, std::map<char, int> &frequencies) {
    for(auto it : frequencies) {
        std::string code = tree.getPrefixCode(it.first);
        prefixCodes[it.first] = {
            static_cast<int>(code.size()),
            packBits(code)
        };
    }
    return 0;
}

int addHeader(std::ofstream &out, std::ifstream &file, std::map<char, prefix> &prefixCodes) {
    int ret = 0, origBytes = 0, tblBytes = 0;
    char ch;
    file.seekg(0, std::ios::beg);

    while(file.get(ch)) {
        origBytes++;
    }
    std::uint64_t origSize = static_cast<uint64_t>(origBytes);
    out.write(reinterpret_cast<const char*>(&origSize), sizeof(origSize));

    for(auto it : prefixCodes) {
        std::size_t entryBytes = 2 + it.second.prefixCode.size();
        tblBytes += entryBytes;
    }
    std::uint16_t tblSize = static_cast<uint16_t>(tblBytes);
    out.write(reinterpret_cast<const char*>(&tblSize), sizeof(tblSize));

    return ret;
}

int addTable(std::ofstream &out, std::ifstream &file, std::map<char, prefix> &prefixCodes) {
    int ret = 0;
    for(auto it : prefixCodes) {
        uint8_t chr = static_cast<uint8_t>(it.first);
        out.write(reinterpret_cast<const char*>(&chr), sizeof(chr));
        uint8_t len = static_cast<uint8_t>(it.second.length);
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        for(auto it1 : it.second.prefixCode) {
            out.write(reinterpret_cast<const char*>(&it1), sizeof(it1));
        }
    }
    return ret;
}

int addPayload(std::ofstream &out, std::ifstream &file, std::map<char, prefix> &prefixCodes) {
    int ret = 0;
    char ch;
    file.seekg(0, std::ios::beg);
    while(file.get(ch)) {
        for(auto it : prefixCodes[ch].prefixCode) {
            out.write(reinterpret_cast<const char*>(&it), sizeof(it));
        }
    }
    return ret;
}

int generateEncodedFile(fs::path &outfile, std::ifstream &file, std::map<char, prefix> &prefixCodes) {
    std::ofstream out(outfile, std::ios::binary);
    int ret = 0;

    file.seekg(0, std::ios::beg);
    if(!out.is_open()) {
        std::cerr << "Unable to open output file" << std::endl;
        ret = -1;
    }
    if(ret == 0) ret = addHeader(out, file, prefixCodes);

    if(ret == 0) ret = addTable(out, file, prefixCodes);

    if(ret == 0) ret = addPayload(out, file, prefixCodes);

    return ret;
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::map<char, int> frequencies;
    std::map<char, prefix> prefixCodes;
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
        std::cout << outpath << std::endl;

        if(ret == 0) ret = generateEncodedFile(outpath, file, prefixCodes);
    }

    return ret;
}