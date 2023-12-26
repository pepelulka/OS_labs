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

// ms
const size_t PING_TIME = 500;

int CreateProcess(const std::string &path, NodeId id, Port pullPort, Port pushPort, Port pingPort);

}
