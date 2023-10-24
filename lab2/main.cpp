#include "timer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

extern "C" {
    #include "merge_sort.h"
};

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

// int main() {
//     const int N = 3;
//     int a[] = {5, 6, 7, 4, 3, 2, 4};
//     ParallelMergeSort(a, 7, N);
//     for (int i = 0; i < 7; i++) {
//         std::cout << a[i] << " ";
//     }
//     std::cout << std::endl;
// }

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Missing arguments!\n";
        exit(EXIT_FAILURE);
    }

    const int N = 3;

    std::string fileName = std::string(argv[1]);
    std::vector<int> a = ReadFromFile(fileName);

    int n = a.size();
    int* aPtr = (int*)malloc(n * sizeof(int));
    int* extraPtr = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        aPtr[i] = a[i];
    }

    Timer timer;

    MergeSort(aPtr, n, extraPtr);
    int time1 = timer.get();

    for (int i = 0; i < n; i++) {
        aPtr[i] = a[i];
    }

    timer.reset();

    ParallelMergeSort(aPtr, n, N);
    int time2 = timer.get();

    // for (int i = 0; i < n; i++) {
    //     std::cout << aPtr[i] << " ";
    // }
    // std::cout << std::endl;
    std::cout << "Time 1: " << time1 << "ms\n";
    std::cout << "Time 2: " << time2 << "ms\n";
}
