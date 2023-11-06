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

bool Check(const std::vector<int> &vec) {
    for (size_t i = 0; i + 1 < vec.size(); i++) {
        if (vec[i + 1] < vec[i]) {
            return false;
        }
    }
    return true;
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

    TIMER_INIT;

    std::vector<int> a = ReadFromFile(filename);
    std::vector<int> b = a;
    std::cout << "Read vector from file " << filename << std::endl;
    
    TIMER_START;
    MergeSort(a);
    TIMER_END;

    TIMER_START;
    ParallelMergeSort(b, threadNum);
    TIMER_END;

    assert(Check(a));
    assert(Check(b));
}
