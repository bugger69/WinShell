#include <iostream>
#include <vector>
#include <filesystem>

#define WC_MODE_DEF 0
#define WC_MODE_SIZE 1
#define WC_MODE_LINES 2
#define WC_MODE_WORDS 3
#define WC_MODE_CHARS 4

namespace fs = std::filesystem;

class Params {
public:
    int mode = WC_MODE_DEF;
};