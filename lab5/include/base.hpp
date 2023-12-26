#pragma once

#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <unistd.h>

namespace lab5 {

using NodeId = size_t;
using Port = size_t;

const Port LOWER_PORT = 5555;
const Port UPPER_PORT = 5600;

int CreateProcess(const std::string &path, NodeId id, Port pullPort, Port pushPort) {
    int pid = fork();
    if (pid == 0) {
        execl(path.c_str(), path.c_str(), std::to_string(id).c_str(), std::to_string(pullPort).c_str(), std::to_string(pushPort).c_str(), nullptr);
    }
    return pid;
}

}
