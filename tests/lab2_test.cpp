#include <gtest/gtest.h>

#include "merge_sort.h"
#include "generator.h"
#include "timer.h"

using namespace lab2;
using namespace gen;

constexpr size_t N = 100000;

TEST(Lab2, SortedTest) {
    std::vector<int> vec = GenerateSorted(N);
    MergeSort(vec);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));

    vec = GenerateSorted(N);
    ParallelMergeSort(vec, 2);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    vec = GenerateSorted(N);
    ParallelMergeSort(vec, 8);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST(Lab2, ReversedTest) {
    std::vector<int> vec = GenerateReversed(N);
    MergeSort(vec);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));

    vec = GenerateReversed(N);
    ParallelMergeSort(vec, 2);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    vec = GenerateReversed(N);
    ParallelMergeSort(vec, 8);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
}

TEST(Lab2, RandomShuffleTest) {
    std::vector<int> vec = GenerateRandomShuffle(N);
    MergeSort(vec);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));

    vec = GenerateRandomShuffle(N);
    ParallelMergeSort(vec, 2);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
    vec = GenerateRandomShuffle(N);
    ParallelMergeSort(vec, 8);
    EXPECT_TRUE(std::is_sorted(vec.begin(), vec.end()));
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
        EXPECT_TRUE(std::is_sorted(vec1.begin(), vec1.end()));
    }

    for (size_t i = 0; i < N_TESTS; i++) {
        vec1 = GenerateRandomShuffle(N);
        timer.reset();
        ParallelMergeSort(vec1, N_THREADS);
        time2 += timer.get();
        EXPECT_TRUE(std::is_sorted(vec1.begin(), vec1.end()));
    }

    EXPECT_GT(time1, time2);
}
