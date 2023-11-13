#include <iostream>

#include "include/parent.h"

using namespace lab3;

int main() {
    std::string pathToChild;
    if (getenv("PATH_TO_CHILD")) {
        pathToChild = std::string(getenv("PATH_TO_CHILD"));
    } else {
        pathToChild = "child.exe";
        std::cout << "Warning: PATH_TO_CHILD is not specified. Using default value 'child.exe'\n";
    }
    ParentRoutine("child.exe", std::cin);
}
