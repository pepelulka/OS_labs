#include <gtest/gtest.h>

#include <iostream>
#include <algorithm>

extern "C" {
    #include "lib.h"
}

constexpr const float EPS = 1e-6;
constexpr const float PI = 3.14159265359;

bool EqualFloat(float lhs, float rhs) {
    return std::abs(lhs - rhs) < EPS;
}

TEST(Lab4, DerivativeTestLib2) {
    EXPECT_TRUE(EqualFloat(Derivative(PI, EPS), 0));
}

TEST(Lab4, SortTestLib2) {
    srand(time(NULL));
    const int N = 100;
    int a[N];
    for (int i = 0; i < N; i++) {
        a[i] = rand();
    }
    Sort(a, N);
    EXPECT_TRUE(std::is_sorted(a, a + N));
}
