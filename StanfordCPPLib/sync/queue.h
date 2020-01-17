//
// Created by schrodinger on 1/17/20.
//

#ifndef STANFORD_QUEUE_H
#define STANFORD_QUEUE_H

#include <sync/utils.h>
#include <algorithm>
#include <atomic>
#include <array>

#if __cplusplus >= 201703L
#include <optional>
template <class T>
using optional = std::optional<T>;
#define _opt std
#else

#include <absl/types/optional.h>

template<class T>
using optional = absl::optional<T>;
#define _opt absl
#endif

template<class T>
struct Slot {
    std::atomic_size_t stamp{0};
    T value;
};

template<class T, size_t Capacity>
class ArrayQueue {
    CachePadded<std::atomic_size_t> head, tail;
    std::array<Slot<T>, Capacity> buffer;
    constexpr const static size_t one_lap = next_power_of_two(Capacity);
public:
    ArrayQueue() noexcept;

    bool push(const T &value) noexcept;

    optional<T> pop() noexcept;

    constexpr size_t capacity() const noexcept;

    bool is_empty() const noexcept;

    bool is_full() const noexcept;

    size_t size() const noexcept;
};

template<class T, size_t Capacity>
ArrayQueue<T, Capacity>::ArrayQueue() noexcept : head(0), tail(0) {
    static_assert(std::is_destructible<T>::value, "T must be destructible!");
    static_assert(Capacity > 0, "Capacity must be greater than zero!");
    for (size_t i = 0; i < Capacity; ++i) {
        buffer[i].stamp.store(i, std::memory_order_relaxed);
    }
}


template<class T, size_t Capacity>
bool ArrayQueue<T, Capacity>::push(const T &value) noexcept {
    auto back_off = BackOff();
    while (true) {
        auto t = tail.load(std::memory_order_relaxed);
        auto index = t & (one_lap - 1);
        auto lap = t & ~(one_lap - 1);
        auto &slot = buffer[index];
        auto stamp = slot.stamp.load(std::memory_order_acquire);
        if (t == stamp) {
            auto new_tail = (index + 1 < Capacity) ? t + 1 : lap + one_lap;
            if (tail.compare_exchange_weak(t, new_tail, std::memory_order_seq_cst, std::memory_order_relaxed)) {
                new(&slot.value) T(value);
                slot.stamp.store(t + 1, std::memory_order_release);
                return true;
            } else {
                back_off.spin();
            }
        } else if ((stamp + one_lap) == t + 1) {
            std::atomic_thread_fence(std::memory_order_seq_cst);
            auto h = head.load(std::memory_order_relaxed);
            if (h + one_lap == t) {
                return false;
            }
            back_off.spin();
        } else {
            back_off.snooze();
        }
    }
}

template<class T, size_t Capacity>
optional<T> ArrayQueue<T, Capacity>::pop() noexcept {
    auto back_off = BackOff();
    while (true) {
        auto h = head.load(std::memory_order_relaxed);
        auto index = h & (one_lap - 1);
        auto lap = h & ~(one_lap - 1);
        auto &slot = buffer[index];
        auto stamp = slot.stamp.load(std::memory_order_acquire);
        if (h + 1 == stamp) {
            auto new_head = ((index + 1) < Capacity) ? h + 1 : lap + one_lap;
            if (head.compare_exchange_weak(h, new_head, std::memory_order_seq_cst, std::memory_order_relaxed)) {
                slot.stamp.store(h + one_lap, std::memory_order_release);
                return {std::move(slot.value)};
            } else {
                back_off.spin();
            }
        } else if (stamp == h) {
            std::atomic_thread_fence(std::memory_order_seq_cst);
            auto t = tail.load(std::memory_order_relaxed);
            if (h == t) {
                return _opt::nullopt;
            }
            back_off.spin();
        } else {
            back_off.snooze();
        }
    }
}

template<class T, size_t Capacity>
constexpr size_t ArrayQueue<T, Capacity>::capacity() const noexcept {
    return Capacity;
}

template<class T, size_t Capacity>
bool ArrayQueue<T, Capacity>::is_empty() const noexcept {
    auto h = head.load(std::memory_order_seq_cst);
    auto t = tail.load(std::memory_order_seq_cst);
    return tail == head;
}

template<class T, size_t Capacity>
bool ArrayQueue<T, Capacity>::is_full() const noexcept {
    auto h = head.load(std::memory_order_seq_cst);
    auto t = tail.load(std::memory_order_seq_cst);
    return tail == head + one_lap;
}

template<class T, size_t Capacity>
size_t ArrayQueue<T, Capacity>::size() const noexcept {
    while (true) {
        auto t = tail.load(std::memory_order_seq_cst);
        auto h = head.load(std::memory_order_seq_cst);
        if (tail.load(std::memory_order_seq_cst) == t) {
            auto hix = h & (one_lap - 1);
            auto tix = t & (one_lap - 1);
            if (hix < tix) {
                return tix - hix;
            } else if (hix > tix) {
                return Capacity - hix + tix;
            } else if (tail == head) {
                return 0;
            } else {
                return Capacity;
            }
        }
    }
}


#endif //STANFORD_QUEUE_H
