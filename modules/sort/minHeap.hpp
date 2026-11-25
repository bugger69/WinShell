#include <vector>
#include <string>

class MinHeap {
private:
    std::vector<std::string> minheap;
protected:
    int heapify();
    std::size_t siftUp(std::size_t index);
    std::size_t siftDown(std::size_t index);
public:
    bool empty();
    int push(std::string str);
    std::string pop();
};