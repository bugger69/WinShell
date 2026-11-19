#include <filesystem>

#define SORT_TYPE_DEF 0
#define SORT_TYPE_INSERT 1

namespace fs = std::filesystem;

class FlagOptions {
public:
    bool unique = false;
    int sortType = SORT_TYPE_DEF;
};