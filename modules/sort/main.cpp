#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include "context.hpp"
#include "minHeap.hpp"

/* Flags and argument processing helpers*/
int setFlagOpts(std::vector<std::string> &flags, FlagOptions &opts) {
    for(auto it : flags) {
        if(it == "-u") {
            opts.unique = true;
        } else if (it.starts_with("--sort=")) {
            std::string pre = it.substr(0, 6);
            std::string sort = it.substr(7, it.size());
            if(sort == "insertion") {
                opts.sortType = SORT_TYPE_INSERT;
            } else if (sort == "merge") {
                opts.sortType = SORT_TYPE_MERGE;
            } else if (sort == "quick") {
                opts.sortType = SORT_TYPE_QUICK;
            } else if (sort == "heap") {
                opts.sortType = SORT_TYPE_HEAP;
            } else if (sort == "radis") {
                opts.sortType = SORT_TYPE_RADIX;
            } else {
                std::cerr << "sort: Invalid sort type" << std::endl;
                return -1;
            }
        }
    }
    return 0;
}

int extractArgsAndFlags(std::vector<std::string> &args, std::vector<std::string> &arg, std::vector<std::string> &flags) {
    for(auto it : args) {
        if(it.starts_with('-')) {
            flags.push_back(it);
        } else {
            arg.push_back(it);
        }
    }
    return 0;
}

/* Insertion sort helpers */
int sortedInsert(std::vector<std::string> &output, std::string &line) {
    for(std::size_t i = 0; i < output.size(); i++) {
        if(line < output[i]) {
            output.insert(output.begin() + i, line);
            return 0;
        }
    }
    output.push_back(line);
    return 0;
}

/* Merge sort helpers */
int sortedMerge(std::vector<std::string> &output, std::size_t left, std::size_t mid, std::size_t right) {
    std::vector<std::string> arr1(output.begin() + left, output.begin() + mid), arr2(output.begin() + mid, output.begin() + right);
    std::size_t s1 = arr1.size(), s2 = arr2.size(), i = left, a = 0, b = 0;
    while(a < s1 && b < s2) {
        if(arr1[a] > arr2[b]) {
            output[i] = arr2[b];
            b++;
        } else {
            output[i] = arr1[a];
            a++;
        }
        i++;
    }
    if(a < s1) {
        while(a < s1) {
            output[i] = arr1[a];
            i++;
            a++;
        }
    }
    if(b < s2) {
        while(b < s2) {
            output[i] = arr2[b];
            i++;
            b++;
        }
    }
    return 0;
}

int mergeSort(std::vector<std::string> &output, std::size_t first, std::size_t last) {
    int ret = 0;
    if(first >= last - 1) return ret;
    std::size_t mid = first + (last - first) / 2;
    ret = mergeSort(output, first, mid);
    if(ret == 0) ret = mergeSort(output, mid, last);
    if(ret == 0) ret = sortedMerge(output, first, mid, last);
    return ret;
}

/* Quick Sort Helper */
int getPivot(std::size_t first, std::size_t last) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distrib(first, last - 1);
    int pivot = distrib(gen);
    return pivot;
}

int partition(std::vector<std::string> &output, std::size_t first, std::size_t last) {
    int pivot = getPivot(first, last);
    std::string pivEl = output[pivot];
    int i = first, j = first;
    std::swap(output[pivot], output[last - 1]);
    while(j < last - 1) {
        if(output[j] <= pivEl) {
            std::swap(output[i], output[j]);
            i++;
        }
        j++;
    }
    pivot = i;
    std::swap(output[i], output[last - 1]);
    return pivot;
}

int quickSort(std::vector<std::string> &output, std::size_t first, std::size_t last) {
    if(last - first <= 1) return 0;
    int pivot = partition(output, first, last);
    quickSort(output, first, pivot);
    quickSort(output, pivot + 1, last);
    return 0;
}

/* Heap Sort Handler*/
int heapSort(std::vector<std::string> &output) {
    MinHeap heap;
    for(auto it : output) {
        heap.push(it);
    }
    output.clear();
    while(!heap.empty()) {
        output.push_back(heap.pop());
    }
    return 0;
}

/* Radix Sort Handler */
int radixSort(std::vector<std::string> &output) {
    return 0;
}

/* Sort Handler */
int sortWithType(std::istream &strm, std::vector<std::string> &output, FlagOptions &opts) {
    std::string line;
    while(getline(strm, line)) {
        if(opts.sortType == SORT_TYPE_INSERT) {
            if(sortedInsert(output, line)) {
                std::cerr << "sort: unable to insert" << std::endl;
                return -1;
            }
        } else output.push_back(line);
    }

    if(opts.sortType == SORT_TYPE_DEF) {
        sort(output.begin(), output.end());
    } else if (opts.sortType == SORT_TYPE_MERGE) {
        mergeSort(output, 0, output.size());
    } else if (opts.sortType == SORT_TYPE_QUICK) {
        quickSort(output, 0, output.size());
    } else if (opts.sortType == SORT_TYPE_HEAP) {
        heapSort(output);
    } else if (opts.sortType == SORT_TYPE_RADIX) {

    }

    return 0;
}

int sortFile(std::vector<std::string> &arg, FlagOptions &opts) {
    fs::path filepath(arg[0]);
    std::ifstream file(filepath);
    std::vector<std::string> sortedFile;
    std::string prev;
    bool hasPrev = false;

    if(sortWithType(file, sortedFile, opts)) {
        std::cerr << "sort: unable to sort" << std::endl;
    }

    for(auto it : sortedFile) {
        if(opts.unique) {
            if(!hasPrev || it != prev) {
                std::cout << it << std::endl;
            }
            prev = it;
            hasPrev = true;
        } else {
            std::cout << it << std::endl;
        }
    }

    return 0;
}

int sortStream(std::vector<std::string> &arg, FlagOptions &opts) {
    return 0;
}

int main (int argc, char* argv[]) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::vector<std::string> arg, flags;
    FlagOptions opts;
    int ret = 0;

    ret = extractArgsAndFlags(args, arg, flags);
    
    ret = setFlagOpts(flags, opts);

    if(arg.size() >= 1) {
        if(ret == 0) ret = sortFile(arg, opts);
    } else {
        if(ret == 0) ret = sortStream(arg, opts); 
    }

    return ret;
}