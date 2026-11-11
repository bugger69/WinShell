#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include "context.hpp"
#include "prefixTree.hpp"
#include "encoder.hpp"
#include "decoder.hpp"


int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::map<char, int> frequencies;
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
        std::map<char, prefix> prefixCodes;

        if(ret == 0) ret = populateFrequencies(file, frequencies);

        if(ret == 0) ret = populateNodes(frequencies, nodes);

        tree.insert(nodes);
    
        if(ret == 0) ret = populatePrefix(tree, prefixCodes, frequencies);
        outpath += ".cmpr";
        std::cout << outpath << std::endl;

        if(ret == 0) ret = generateEncodedFile(outpath, file, prefixCodes);
    } else if (args.size() > 1 && args[1] == "decode") {
        FileHeader head;
        std::map<prefix, char> prefixCodes;

        if(ret == 0) ret = extractHeader(file, head);

        if(ret == 0) ret = extractTable(file, head, prefixCodes);

        if(ret == 0) ret = extractPayloadAndWriteFile(file, head, prefixCodes);
    } else {
        ret = -1;
    }

    return ret;
}