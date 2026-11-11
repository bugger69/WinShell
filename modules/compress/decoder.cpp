#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "encoder.hpp"

int extractHeader(fs::path &filepath, FileHeader &head) {
    return 0;
}

int extractTable(fs::path &filepath, FileHeader &head, std::map<prefix, char> &prefix) {
    return 0;
}

int extractPayloadAndWriteFile(fs::path &filepath, FileHeader &head, std::map<prefix, char> &prefix) {
    return 0;
}