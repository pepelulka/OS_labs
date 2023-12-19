#pragma once

#include <algorithm>
#include <fstream>
#include <vector>
#include <random>

namespace gen {

void WriteToFile(const std::vector<int> &vec, const std::string &filename);
std::vector<int> GenerateSorted(size_t n);
std::vector<int> GenerateReversed(size_t n);
std::vector<int> GenerateRandomShuffle(size_t n);

}
