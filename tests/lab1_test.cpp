#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

extern "C" {
    #include "parent.h"
}

TEST(Lab1, SimpleTest) {
    std::string fileWithInput = "input.txt";
    std::string fileWithOutput1 = "output1.txt";
    std::string fileWithOutput2 = "output2.txt";

    constexpr int inputSize = 5;

    std::array<std::string, inputSize> input = {
        "fffff",
        "glazaglaza",
        "piiiiiiiiiiii",
        "kumaroid3000",
        "meow meow meow"
    };

    std::array<std::string, inputSize> expectedOutput = {
        "fffff",
        "glzglz",
        "p"
        "kmrd3000",
        "mw mw mw"
    };

    {
        auto inFile = std::ofstream(fileWithInput);

        inFile << fileWithOutput1 << '\n';
        inFile << fileWithOutput2 << '\n';

        for(const auto& line : input) {
            inFile << line << '\n';
        }
    }

    auto deleter = [](FILE* file) {
        fclose(file);
    };

    std::unique_ptr<FILE, decltype(deleter)> inFile(fopen(fileWithInput.c_str(), "r"), deleter);

    if (getenv("PATH_TO_CHILD") == NULL) {
        printf("WARNING: PATH_TO_CHILD was not specified.\n");
    }
    ParentRoutine(getenv("PATH_TO_CHILD"), inFile.get());

    auto outFile1 = std::ifstream(fileWithOutput1);
    auto outFile2 = std::ifstream(fileWithOutput2);

    ASSERT_TRUE(outFile1.good());
    ASSERT_TRUE(outFile2.good());

    std::vector<std::string> output;
    for (std::string line; std::getline(outFile1, line); ) {
        output.push_back(line);
    }
    for (std::string line; std::getline(outFile2, line); ) {
        output.push_back(line);
    }

    std::sort(output.begin(), output.end());

    EXPECT_EQ(output.size(), inputSize);
    for (int i = 0; i < inputSize; i++) {
        EXPECT_EQ(output[i], expectedOutput[i]);
    }

    auto removeIfExists = [](const std::string& path) {
        if (fs::exists(path)) {
            fs::remove(path);
        }
    };

    removeIfExists(fileWithInput);
    removeIfExists(fileWithOutput1);
    removeIfExists(fileWithOutput2);
}
