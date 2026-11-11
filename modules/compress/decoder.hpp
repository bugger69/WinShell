#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "prefixTree.hpp"
#include "context.hpp"

namespace fs = std::filesystem;

int extractHeader(std::ifstream &file, FileHeader &head);
int extractTable(std::ifstream &file, FileHeader &head, std::map<prefix, char> &prefix);
int extractPayloadAndWriteFile(std::ifstream &file, FileHeader &head, std::map<prefix, char> &prefix);
