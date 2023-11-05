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

#define TIMER_INIT Timer my_global_timer
#define TIMER_START my_global_timer.reset()
#define TIMER_END std::cout << "Timer: " << my_global_timer.get() << "ms\n"

} // namespace lab2
