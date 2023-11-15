#include "lib.h"

#include <math.h>

float Derivative(float a, float deltaX) {
    return (cos(a + deltaX) - cos(a - deltaX)) / (2 * deltaX);
}

static void swap(int* lhs, int* rhs) {
    int c = *lhs;
    *lhs = *rhs;
    *rhs = c;
}

static void QuickSort(int* array, int first, int last) {
    int i = first, j = last, x = array[(first + last) / 2];

    do {
        while (array[i] < x) i++;
        while (array[j] > x) j--;

        if(i <= j) {
            if (array[i] > array[j]) swap(&array[i], &array[j]);
            i++;
            j--;
        }
    } while (i <= j);

    if (i < last) {
        QuickSort(array, i, last);
    }
    if (first < j) {
        QuickSort(array, first, j);
    }
}

int* Sort(int * array, size_t size) {
    if (size == 1 || size == 0) {
        return array;
    }
    QuickSort(array, 0, size - 1);
    return array;
}
