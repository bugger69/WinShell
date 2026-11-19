#include <filesystem>

#define SORT_TYPE_INSERT 1

namespace fs = std::filesystem;

class FlagOptions {
public:
    bool unique = false;
    int sort = SORT_TYPE_INSERT;
};