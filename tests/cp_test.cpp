#include "gtest/gtest.h"

#include "job_exec.hpp"

std::string getPath() {
    if (getenv("PATH_TO_EX_DIR") == nullptr) {
        std::cout << "PATH_TO_EX_DIR is not specified\n";
        exit(EXIT_FAILURE);
    }
    std::string path = std::string(getenv("PATH_TO_EX_DIR"));
    return path;
}

TEST(CP, Test1) {
    std::string path = getPath() + "ex1/ex1.json";
    cp::TJobDag dag = cp::JSONParser::Parse(path);
    cp::TDagJobExecutor ex(4);
    EXPECT_TRUE(ex.Execute(dag));
}

TEST(CP, Test2) {
    std::string path = getPath() + "ex2/ex2.json";
    cp::TJobDag dag = cp::JSONParser::Parse(path);
    cp::TDagJobExecutor ex(10);
    ex.Execute(dag);
    EXPECT_TRUE(ex.Execute(dag));
}

TEST(CP, Test3) {
    std::string path = getPath() + "ex4/ex4.json";
    cp::TJobDag dag = cp::JSONParser::Parse(path);
    cp::TDagJobExecutor ex(1);
    ex.Execute(dag);
    EXPECT_TRUE(ex.Execute(dag));
}

TEST(CP, Test4) {
    std::string path = getPath() + "ex5/ex5.json";
    cp::TJobDag dag = cp::JSONParser::Parse(path);
    cp::TDagJobExecutor ex(4);
    ex.Execute(dag);
    EXPECT_TRUE(ex.Execute(dag));
}

TEST(CP, Test5) {
    std::string path = getPath() + "ex7/ex7.json";
    EXPECT_THROW({cp::TJobDag dag = cp::JSONParser::Parse(path);}, std::logic_error);
}

