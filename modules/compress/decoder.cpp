#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "encoder.hpp"

int extractHeader(std::ifstream &file, FileHeader &head) {
    int ret = 0;

    return ret;
}

int extractTable(std::ifstream &file, FileHeader &head, std::map<prefix, char> &prefix) {
    return 0;
}

int extractPayloadAndWriteFile(std::ifstream &file, FileHeader &head, std::map<prefix, char> &prefix) {
    return 0;
}