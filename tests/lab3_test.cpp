#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <memory>

#include "parent.h"

#ifdef _WIN32

#include "windows.h"

#endif

namespace fs = std::filesystem;

using TStringVector = std::vector<std::string>;

using namespace lab3;

const std::string inFileName = "input.txt";
const std::string outFile1Name = "output1.txt";
const std::string outFile2Name = "output2.txt";

void RemoveFileIfExists (const std::string& path) {
#ifdef  _WIN32
    if (fs::exists(path)) {
        DeleteFileA(path.c_str());
    }
#else
    if (fs::exists(path)) {
        fs::remove(path);
    }
#endif
}

void Check(const TStringVector& input,
           TStringVector expectedOutput) {
    size_t inputSize = input.size();

    {
        auto inFile = std::ofstream(inFileName);

        inFile << outFile1Name << '\n';
        inFile << outFile2Name << '\n';

        for(const auto& line : input) {
            inFile << line << '\n';
        }
    }

    std::ifstream inFile(inFileName);

    if (getenv("PATH_TO_CHILD") == NULL) {
        printf("WARNING: PATH_TO_CHILD was not specified.\n");
    }
    ParentRoutine(getenv("PATH_TO_CHILD"), inFile);

    auto outFile1 = std::ifstream(outFile1Name);
    auto outFile2 = std::ifstream(outFile2Name);

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

    inFile.close();
    outFile1.close();
    outFile2.close();
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

    Check(input, expectedOutput);
}

TEST(Lab1, Test2) {
    TStringVector input = {  };

    TStringVector expectedOutput = {  };

    Check(input, expectedOutput);
}

TEST(Lab1, Test3) {
    TStringVector input = { "" };

    TStringVector expectedOutput = { "" };

    Check(input, expectedOutput);
}

TEST(Lab1, Test4) {
    TStringVector input = { "bsakk" };

    TStringVector expectedOutput = { "bskk" };

    Check(input, expectedOutput);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    RemoveFileIfExists(inFileName);
    RemoveFileIfExists(outFile1Name);
    RemoveFileIfExists(outFile2Name);
    return result;
}
