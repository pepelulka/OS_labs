#include <iostream>
#include <sstream>

#include "base.hpp"
#include "computer_base.hpp"

#include "zmq.hpp"

using namespace lab5;

int main(int argc, char ** argv) {

    if (argc < 5) {
        std::cerr << "Missing arguments!\n";
        exit(EXIT_FAILURE);
    }
    NodeId id = std::atoi(argv[1]);
    Port pullPort = std::atoi(argv[2]), pushPort = std::atoi(argv[3]), pingPort = std::atoi(argv[4]);

    TComputerNode node(std::string(getenv("PATH_TO_COMP")), id, pullPort, pushPort, pingPort);
    node.Routine();
}