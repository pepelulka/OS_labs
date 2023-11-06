#include <gtest/gtest.h>

#include "merge_sort.h"
#include "generator.h"
#include "timer.h"

using namespace lab2;
using namespace gen;

bool Check(const std::vector<int> &vec) {
    for (size_t i = 0; i + 1 < vec.size(); i++) {
        if (vec[i + 1] < vec[i]) {
            return false;
        }
    }
    return true;
}

TEST(Lab2, BasicTest) {
    constexpr size_t N = 100000;

    std::vector<int> vec1 = GenerateSorted(N);
    std::vector<int> vec2 = GenerateReversed(N);
    std::vector<int> vec3 = GenerateRandomShuffle(N);
    MergeSort(vec1);
    MergeSort(vec2);
    MergeSort(vec3);
    EXPECT_TRUE(Check(vec1));
    EXPECT_TRUE(Check(vec2));
    EXPECT_TRUE(Check(vec3));

    vec1 = GenerateSorted(N);
    vec2 = GenerateReversed(N);
    vec3 = GenerateRandomShuffle(N);
    ParallelMergeSort(vec1, 2);
    ParallelMergeSort(vec2, 4);
    ParallelMergeSort(vec3, 8);
    EXPECT_TRUE(Check(vec1));
    EXPECT_TRUE(Check(vec2));
    EXPECT_TRUE(Check(vec3));
}

TEST(Lab2, SpeedTest) {
    constexpr size_t N = 1000000;
    constexpr size_t N_TESTS = 5;
    constexpr size_t N_THREADS = 4;

    Timer timer;

    size_t time1 = 0;
    size_t time2 = 0;
    std::vector<int> vec1;
    for (size_t i = 0; i < N_TESTS; i++) {
        vec1 = GenerateRandomShuffle(N);
        timer.reset();
        MergeSort(vec1);
        time1 += timer.get();
        EXPECT_TRUE(Check(vec1));
    }

    for (size_t i = 0; i < N_TESTS; i++) {
        vec1 = GenerateRandomShuffle(N);
        timer.reset();
        ParallelMergeSort(vec1, N_THREADS);
        time2 += timer.get();
        EXPECT_TRUE(Check(vec1));
    }

    EXPECT_TRUE(time1 > time2);
}
