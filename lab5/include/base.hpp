#include <iostream>

#include <unistd.h>

namespace lab5 {

using NodeId = size_t;
using Port = size_t;

const Port BASE_PORT = 5555;
const size_t MAX_CONNECTIONS = 20;

void StartComputer(NodeId id, Port port, const std::string &pathToComputer) {
    int pid = fork();
    if (pid == -1) {
        throw std::logic_error("Can't fork");
    } else {
        const char* args[] = {std::to_string(id).c_str(),
                              std::to_string(port).c_str(), nullptr};
        execv(pathToComputer.c_str(), const_cast<char* const*>(args));
    }
}

}
