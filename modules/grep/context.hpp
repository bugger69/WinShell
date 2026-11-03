#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class Params {
public:
    bool onlyMatching = false;
};