#pragma once

#include <chrono>
#include <iostream>

namespace lab2 {

class Timer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using Unit = std::chrono::milliseconds;

    Timer();

    void reset();
    int get();
private:
    std::chrono::time_point<Clock> timePoint;
};

} // namespace lab2
