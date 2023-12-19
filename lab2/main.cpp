#include "timer.h"

#include "thread_pool.h"
#include "merge_sort.h"

#include <fstream>
#include <cassert>

using namespace lab2;

std::vector<int> ReadFromFile(const std::string &fileName) {
    std::ifstream in(fileName);
    std::vector<int> result;
    size_t n;
    in >> n;
    result.resize(n);
    for (size_t i = 0; i < n; i++) {
        in >> result[i];
    }
    return result;
}

// Usage:
// <executable> <filename> <thread-count>
int main(int argc, char **argv) {
    int threadNum = 1;
    std::string filename = "vec.txt";
    if (argc >= 2) {
        filename = std::string(argv[1]);
    }
    if (argc >= 3) {
        threadNum = std::atoi(argv[2]);
    }

    Timer timer;

    std::vector<int> a = ReadFromFile(filename);
    std::vector<int> b = a;
    std::cout << "Read vector from file " << filename << std::endl;
    
    timer.reset();
    MergeSort(a);
    std::cout << "time1: " << timer.get() << "ms\n"; 

    timer.reset();
    ParallelMergeSort(b, threadNum);
    std::cout << "time2: " << timer.get() << "ms\n"; 

    assert(std::is_sorted(a.begin(), a.end()));
    assert(std::is_sorted(b.begin(), b.end()));
}
