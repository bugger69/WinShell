#include <vector>
#include <string>

class MinHeap {
private:
    std::vector<std::string> minheap;
protected:
    int heapify();
public:
    bool empty();
    int push(std::string str);
    std::string pop();
};