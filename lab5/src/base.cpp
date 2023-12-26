#include "base.hpp"

namespace lab5 {

int CreateProcess(const std::string &path, NodeId id, Port pullPort, Port pushPort, Port pingPort) {
    int pid = fork();
    if (pid == 0) {
        execl(path.c_str(), path.c_str(), std::to_string(id).c_str(), std::to_string(pullPort).c_str(), std::to_string(pushPort).c_str(), std::to_string(pingPort).c_str(), nullptr);
    }
    return pid;
}

}
