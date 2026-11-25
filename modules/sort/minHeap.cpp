#include <algorithm>
#include "minHeap.hpp"

int MinHeap::heapify() {
    if (this->minheap.size() < 2)
        return 0;

    std::size_t i = this->minheap.size() / 2;
    while(i > 0) {
        i--;
        std::size_t left = 2 * i + 1;
        std::size_t right = 2 * i + 2;
        std::vector<std::size_t> children = {left, right};
        std::size_t toswap = i;
        for(auto it : children) {
            if(this->minheap[toswap] > this->minheap[it]) toswap = it;
        }
        if(toswap != i) std::swap(this->minheap[i], this->minheap[toswap]);
    }
    return 0;
}

std::size_t MinHeap::siftUp(std::size_t index) {
    if(this->minheap.empty()) return -1;
    std::size_t curr = index;
    while (curr > 0) {
        std::size_t root = ((curr + 1) / 2) - 1;
        if(this->minheap[root] > this->minheap[curr]) {
            std::swap(this->minheap[root], this->minheap[curr]);
            curr = root;
        } else break;
    }
    return 0;
}

std::size_t MinHeap::siftDown(std::size_t index) {
    if(this->minheap.empty()) return -1;
    int curr = index;
    while(curr < this->minheap.size()) {
        std::size_t left = curr * 2 + 1;
        std::size_t right = curr * 2 + 2;
        std::vector<std::size_t> children = {left, right};
        int small = curr;
        for(auto it : children) {
            if(this->minheap[small] > this->minheap[it]) small = it;
        }
        if(small != curr) {
            std::swap(this->minheap[curr], this->minheap[small]);
            curr = small;
        } else break;
    }
    return curr;
}

bool MinHeap::empty() {
    return this->minheap.empty();
}

int MinHeap::push(std::string str) {
    this->minheap.push_back(str);
    this->siftUp(this->minheap.size() - 1);
    return 0;
}

std::string MinHeap::pop() {
    if(empty()) return "";

    std::string top = this->minheap[0];
    std::swap(this->minheap[0], this->minheap[this->minheap.size() - 1]);
    this->siftDown(0);
    this->minheap.erase(minheap.begin() + this->minheap.size() - 1);
    return top;
}