//
// Created by schrodinger on 1/17/20.
//

#include "utils.h"

void BackOff::reset() {
    step = 0;
}

void BackOff::spin() {
    auto loops = (SPIN_LIMIT < step) ? (1u << SPIN_LIMIT) : (1u << step);
    for (uint32_t i = 0; i < loops; ++i) {
        spin_loop_hint();
    }
    if (step <= SPIN_LIMIT) {
        step++;
    }
}

void BackOff::snooze() {
    if (step <= SPIN_LIMIT) {
        for (uint32_t i = 0; i < (1u << step); ++i) {
            spin_loop_hint();
        }
    } else {
        std::this_thread::yield();
    }
    if (step <= YIELD_LIMIT) {
        step++;
    }
}

bool BackOff::is_completed() {
    return step > YIELD_LIMIT;
}

