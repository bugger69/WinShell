#include "minHeap.hpp"

int MinHeap::heapify() {
    return 0;
}

bool MinHeap::empty() {
    return this->minheap.empty();
}

int MinHeap::push(std::string str) {
    this->minheap.push_back(str);
    this->heapify();
    return 0;
}

std::string MinHeap::pop() {
    if(empty()) return "";

    std::string top = this->minheap[0];
    this->minheap.erase(this->minheap.begin());
    this->heapify();
    return top;
}