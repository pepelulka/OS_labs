#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <unistd.h>

namespace lab5 {

using namespace std::chrono_literals;

using NodeId = size_t;
using Port = size_t;

const Port LOWER_PORT = 10555;
const Port UPPER_PORT = 10600;

const auto FIRST_PING_TIME = 2000ms;
const auto SECOND_PING_TIME = 500ms;

// For ping we use message "r" for descend and "p" for ascend

int CreateProcess(const std::string &path, NodeId id, Port pullPort, Port pushPort);

}
