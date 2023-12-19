#include "lib.h"

#include <math.h>

float Derivative(float a, float deltaX) {
    return (cos(a + deltaX) - cos(a)) / deltaX;
}

int* Sort(int * array, size_t size) {
    if (size == 1 || size == 0) {
        return array;
    }
    for (size_t i = 0; i < size; i++) {
        for (int j = size - 1; j >= 1; j--) {
            if (array[j] < array[j - 1]) {
                int c = array[j];
                array[j] = array[j - 1];
                array[j - 1] = c;
            }
        }
    }
    return array;
}
