#include <iostream>
#include <vector>
#include <filesystem>

#define WC_MODE_SIZE 0
#define WC_MODE_LINES 1
#define WC_MODE_WORDS 2
#define WC_MODE_CHARS 3

namespace fs = std::filesystem;

class Params {
public:
    int mode = WC_MODE_SIZE;
};