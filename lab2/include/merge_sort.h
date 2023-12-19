#pragma once

#include <iostream>
#include <vector>

#include "thread_pool.h"

namespace lab2 {

void MergeSort(std::vector<int> &vec);
void ParallelMergeSort(std::vector<int> &vec, size_t threadCount);

}
