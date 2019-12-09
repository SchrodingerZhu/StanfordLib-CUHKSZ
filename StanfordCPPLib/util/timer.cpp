/*
 * File: timer.cpp
 * ---------------
 * Implementation of the Timer class as declared in timer.h.
 */

#include "timer.h"

Timer::Timer(bool autostart) {
    _start = std::chrono::system_clock::now();
    _stop = std::chrono::system_clock::now();
    _isStarted = false;
    if (autostart) {
        start();
    }
}

long Timer::elapsed() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(_stop - _start).count();
}

bool Timer::isStarted() const {
    return _isStarted;
}

void Timer::start() {
    _start = std::chrono::system_clock::now();
    _isStarted = true;
}

long Timer::stop() {
    _stop = std::chrono::system_clock::now();
    if (!_isStarted) {
        // error("Timer is not started");
        _start = _stop;
    }
    _isStarted = false;
    return elapsed();
}

long Timer::currentTimeMS() {
    return std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
}
