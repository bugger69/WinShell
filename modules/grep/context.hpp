#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

#define HIGHLIGHT_NONE 0
#define HIGHLIGHT_AUTO 1
#define HIGHLIGHT_ALWAYS 2

class Params {
public:
    bool onlyMatching = false;
    int highlight = HIGHLIGHT_NONE;
    bool recursive = false;
};