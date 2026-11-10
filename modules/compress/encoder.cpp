#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "prefixTree.hpp"
#include "encoder.hpp"

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
    char ch;
    std::uint8_t byte = 0;
    int bitCount = 0;
    file.seekg(0, std::ios::beg);
    while(file.get(ch)) {
        const prefix& code = prefixCodes.at(ch);
        for(int bitIndex = 0; bitIndex < code.length; ++bitIndex) {
            const std::uint8_t bit = static_cast<std::uint8_t>(
                (code.prefixCode[bitIndex / 8] >> (7 - (bitIndex % 8))) & 1);
            byte = static_cast<std::uint8_t>((byte << 1) | bit);

            if(++bitCount == 8) {
                out.put(static_cast<char>(byte));
                if(!out) {
                    std::cerr << "Unable to write encoded payload" << std::endl;
                    return -1;
                }
                byte = 0;
                bitCount = 0;
            }
        }
    }

    if(bitCount > 0) {
        out.put(static_cast<char>(byte << (8 - bitCount)));
        if(!out) {
            std::cerr << "Unable to write encoded payload" << std::endl;
            return -1;
        }
    }

    return 0;
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
