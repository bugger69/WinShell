#ifndef COMPRESS_CONTEXT_HPP
#define COMPRESS_CONTEXT_HPP

#include <vector>
#include <cstdint>

struct prefix {
    int length;
    std::vector<std::uint8_t> prefixCode;

    bool operator<(const prefix& other) const {
        if (length != other.length) return length < other.length;
        return prefixCode < other.prefixCode;
    }
};

struct FileHeader {
    uint64_t origlen;
    uint16_t tblen;
};

#endif // COMPRESS_CONTEXT_HPP