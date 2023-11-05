#pragma once

#include <iostream>
#include <vector>

#include "task_queue.h"

namespace lab2 {

void MergeSort(std::vector<int> &vec);
void ParallelMergeSort(std::vector<int> &vec, size_t threadCount);

}
