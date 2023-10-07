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

using TStringVector = std::vector<std::string>;

// const std::string fileWithInput = "input.txt";
// const std::string fileWithOutput1 = "output1.txt";
// const std::string fileWithOutput2 = "output2.txt";

void Check(const TStringVector& input,
           TStringVector expectedOutput,
           std::string fileWithInput,
           std::string fileWithOutput1,
           std::string fileWithOutput2) {
    size_t inputSize = input.size();

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

    std::sort(expectedOutput.begin(), expectedOutput.end());
    std::sort(output.begin(), output.end());

    ASSERT_TRUE(output.size() == inputSize);
    for (size_t i = 0; i < output.size(); i++) {
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

TEST(Lab1, Test1) {
    TStringVector input = {
        "",
        "fffff",
        "glazaglaza",
        "kumaroid3000",
        "meow meow meow",
        "piiiiiiiiiiii"
    };

    TStringVector expectedOutput = {
        "",
        "fffff",
        "glzglz",
        "kmrd3000",
        "mw mw mw",
        "p"
    };

    Check(input, expectedOutput, "input1.txt", "output11.txt", "output12.txt");
}

TEST(Lab1, Test2) {
    TStringVector input = {  };

    TStringVector expectedOutput = {  };

    Check(input, expectedOutput, "input2.txt", "output21.txt", "output22.txt");
}

TEST(Lab1, Test3) {
    TStringVector input = { "" };

    TStringVector expectedOutput = { "" };

    Check(input, expectedOutput, "input3.txt", "output31.txt", "output32.txt");
}

TEST(Lab1, Test4) {
    TStringVector input = { "bsakk" };

    TStringVector expectedOutput = { "bskk" };

    Check(input, expectedOutput, "input4.txt", "output41.txt", "output42.txt");
}
