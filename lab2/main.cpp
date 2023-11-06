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

int main(int argc, char **argv) {
    int threadNum = 1;
    if (argc == 2) {
        threadNum = std::atoi(argv[1]);
    }

    TIMER_INIT;

    std::vector<int> a = ReadFromFile("vec.txt");
    std::vector<int> b = a;
    std::cout << "Read vector from file vec.txt\n";
    
    TIMER_START;
    MergeSort(a);
    TIMER_END;

    TIMER_START;
    ParallelMergeSort(b, threadNum);
    // for (auto i : b) {
    //     std::cout << i << " ";
    // }
    // std::cout << std::endl;
    TIMER_END;

    assert(Check(a));
    assert(Check(b));
}
