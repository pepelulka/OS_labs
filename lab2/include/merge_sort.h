#ifndef _MERGE_SORT_
#define _MERGE_SORT_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void MergeSort(int *array, size_t size, int *extra);
void ParallelMergeSort(int *array, size_t size, size_t threadCount);

#endif
