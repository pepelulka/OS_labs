#include <iostream>

#include "include/parent.h"

using namespace lab3;

int main() {
    char* pathToChild;
    if (getenv("PATH_TO_CHILD")) {
        pathToChild = getenv("PATH_TO_CHILD");
    } else {
        std::cerr << "Error: PATH_TO_CHILD is not specified.\n";
        exit(EXIT_FAILURE);
    }
    ParentRoutine(pathToChild, std::cin);
}
