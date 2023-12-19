#include "../include/parent.h"
#include "../include/util.h"

#include <string>

#ifdef _WIN32

namespace lab3 {

const std::string mmap1Name = "pepe_mmap_1";
const std::string mmap2Name = "pepe_mmap_2";

void ParentRoutine(const char* pathToChild, std::istream &in) {
    const size_t firstProcessProbability = 80;

    util::Randomizer rand;
    std::string file1, file2;
    std::getline(in, file1);
    std::getline(in, file2);

    util::TMMap map1('w', mmap1Name, 10000), map2('w', mmap2Name, 10000);

    for (std::string line; std::getline(in, line);) {
        // One extra empty string
        if (rand.Probability(firstProcessProbability)) {
            map1.Write(line + "\n");
        } else {
            map2.Write(line + "\n");
        }
    }

    util::Process child1(pathToChild, {file1, mmap1Name});
    util::Process child2(pathToChild, {file2, mmap2Name});
    child1.Wait();
    child2.Wait();
}

}

#endif
