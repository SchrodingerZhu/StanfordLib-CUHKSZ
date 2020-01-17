//
// Created by schrodinger on 1/17/20.
//

#ifndef STANFORD_UTILS_H
#define STANFORD_UTILS_H

#include <cstddef>
#include <cstdint>
#include <thread>
#include <optional>

constexpr const uint8_t SPIN_LIMIT = 6;
constexpr const uint8_t YIELD_LIMIT = 10;

inline void spin_loop_hint() {
    asm volatile("pause\n": : :"memory");
}

inline constexpr size_t next_power_of_two(size_t z) {
    if (z <= 1) return 1;
    else {
        size_t t = __builtin_clzll(z - 1);
        return (std::numeric_limits<size_t>::max() >> t) + 1;
    }
}

class BackOff {
    uint32_t step = 0;
public:
    void reset();

    void spin();

    void snooze();

    bool is_completed();
};

template<class T>
class alignas(128) CachePadded : public T {
public:
    template<typename... Args>
    CachePadded(Args &&... args);
};

template<class T>
template<typename... Args>
CachePadded<T>::CachePadded(Args &&... args) : T(std::forward<Args>(args)...) {}


#endif //STANFORD_UTILS_H
