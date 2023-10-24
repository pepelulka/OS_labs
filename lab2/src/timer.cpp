#include "timer.h"

namespace lab2 {

Timer::Timer() {
    this->reset();
}

void Timer::reset() {
    this->timePoint = Clock::now();
}

int Timer::get() {
    auto timeNow = Clock::now();
    return std::chrono::duration_cast<Unit>(timeNow - this->timePoint).count();
}

} // namespace lab2
