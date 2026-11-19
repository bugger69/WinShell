#include <filesystem>

#define SORT_TYPE_DEF 0
#define SORT_TYPE_INSERT 1
#define SORT_TYPE_RADIX 2
#define SORT_TYPE_MERGE 3
#define SORT_TYPE_QUICK 4
#define SORT_TYPE_HEAP 5

namespace fs = std::filesystem;

class FlagOptions {
public:
    bool unique = false;
    int sortType = SORT_TYPE_DEF;
};