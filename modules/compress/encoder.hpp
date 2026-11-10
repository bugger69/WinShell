#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "prefixTree.hpp"

namespace fs = std::filesystem;

struct prefix {
    int length;
    std::vector<std::uint8_t> prefixCode;
};

int populateFrequencies(std::ifstream& file, std::map<char, int>& frequencies);
int populateNodes(std::map<char, int>& frequencies, std::vector<Node>& nodes);
int populatePrefix(PrefixTree &tree, std::map<char, prefix> &prefixCodes, std::map<char, int> &frequencies);
int generateEncodedFile(fs::path &outfile, std::ifstream &file, std::map<char, prefix> &prefixCodes);