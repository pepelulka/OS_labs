#include "generator.h"

namespace gen {

void WriteToFile(const std::vector<int> &vec, const std::string &filename) {
    size_t size = vec.size();
    std::ofstream out(filename);
    out << size << std::endl;
    for (const auto &i : vec) {
        out << i << " ";
    }
}

std::vector<int> GenerateSorted(size_t n) {
    std::vector<int> result;
    for (size_t i = 0; i < n; i++) {
        result.push_back(i);
    }
    return result;
}

std::vector<int> GenerateReversed(size_t n) {
    std::vector<int> result;
    for (size_t i = 0; i < n; i++) {
        result.push_back(n - i - 1);
    }
    return result;
}

std::vector<int> GenerateRandomShuffle(size_t n) {
    std::vector<int> result;
    for (size_t i = 0; i < n; i++) {
        result.push_back(i);
    }
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(result), std::end(result), rng);
    return result;
}
    
}

