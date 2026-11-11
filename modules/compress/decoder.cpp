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

    head.origlen = origsize;

    if(ret != 0 || !file.read(reinterpret_cast<char*>(&tblsize), sizeof(tblsize))) {
        std::cerr << "compress: unable to read table size from the header" << std::endl;
        ret = -1;
    }

    head.tblen = tblsize;
    return ret;
}

int extractTable(std::ifstream &file, FileHeader &head, std::map<prefix, char> &prefix) {
    return 0;
}

int extractPayloadAndWriteFile(std::ifstream &file, FileHeader &head, std::map<prefix, char> &prefix) {
    return 0;
}