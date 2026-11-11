#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include "prefixTree.hpp"
#include "context.hpp"

namespace fs = std::filesystem;

int extractHeader(fs::path &filepath, FileHeader &head);
int extractTable(fs::path &filepath, FileHeader &head, std::map<prefix, char> &prefix);
int extractPayloadAndWriteFile(fs::path &filepath, FileHeader &head, std::map<prefix, char> &prefix);
