#include <string>
#include <set>

struct FlagData {
    char delimiter = '\t';
    std::set<int> allowed;
};
