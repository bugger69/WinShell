#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "encoder.hpp"

int extractHeader(std::ifstream &file, FileHeader &head) {
    int ret = 0;
    std::uint64_t origsize{};
    std::uint16_t tblsize{};

    if(!file.read(reinterpret_cast<char*>(&origsize), sizeof(origsize))) {
        std::cerr << "compress: unable to read original size from the header" << std::endl;
        ret = -1;
    }

    if(ret == 0) head.origlen = origsize;

    if(ret != 0 || !file.read(reinterpret_cast<char*>(&tblsize), sizeof(tblsize))) {
        std::cerr << "compress: unable to read table size from the header" << std::endl;
        ret = -1;
    }

    if(ret == 0) head.tblen = tblsize;
    return ret;
}

int extractTable(std::ifstream &file, FileHeader &head, std::map<prefix, char> &pref) {
    int ret = 0, crlen = 0;

    while(crlen < head.tblen) {
        prefix currPre{};
        char ch;
        std::size_t bytecount = 0, i = 0;
        std::uint8_t byte = 0;
        std::uint8_t len{};
        if(!file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
            std::cerr << "compress: unable to get char" << std::endl;
            ret = -1;
            break;
        }
        crlen++;
        if(!file.read(reinterpret_cast<char*>(&len), sizeof(len))) {
            std::cerr << "compress: unable to get length" << std::endl;
            ret = -1;
            break;
        }
        currPre.length = len;
        crlen++;
        bytecount = (static_cast<std::size_t>(len) + 7) / 8;
        for(std::size_t i = 0; i < bytecount; i++) {
            if(!file.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
               std::cerr << "compress: unable to fetch code" << std::endl;
               ret = -1;
               return ret; 
            }
            currPre.prefixCode.push_back(byte);
            crlen++;
        }
        pref[currPre] = ch;
    }

    return ret;
}

int extractPayloadAndWriteFile(std::ifstream &file, FileHeader &head, std::map<prefix, char> &pref, fs::path filepath) {
    int ret = 0;
    std::string outfile("new" + filepath.filename().string());
    fs::path outf(outfile);
    std::ofstream out(outf, std::ios::binary);
    if(!out.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        ret = -1;
    }

    return ret;
}