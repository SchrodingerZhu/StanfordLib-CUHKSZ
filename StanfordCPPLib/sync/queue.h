//
// Created by schrodinger on 1/17/20.
//

#ifndef STANFORD_QUEUE_H
#define STANFORD_QUEUE_H

#include <sync/utils.h>
#include <algorithm>
#include <atomic>
#include <array>
#include <new>

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


template<class T, size_t Capacity>
class ArrayQueue {

    struct Slot {
        std::atomic_size_t stamp{0};
        std::aligned_storage_t<sizeof(T), alignof(T)> value;
    };

    CachePadded<std::atomic_size_t> head, tail;
    std::array<Slot, Capacity> buffer;
    constexpr const static size_t one_lap = next_power_of_two(Capacity);
public:
    ArrayQueue() noexcept;

    bool push(const T &value);

    optional<T> pop() noexcept;

    constexpr size_t capacity() const noexcept;

    bool is_empty() const noexcept;

    bool is_full() const noexcept;

    size_t size() const noexcept;

    template<typename... Args>
    bool emplace(Args &&... args);
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
template<typename ...Args>
bool ArrayQueue<T, Capacity>::emplace(Args &&...args) {
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
                new(&slot.value) T(std::forward<Args>(args)...);
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
                T t(std::move(*(T *) (&slot.value)));
                ((T *) (&slot.value))->~T();
                return {std::move(t)};
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

template<class T, size_t Capacity>
bool ArrayQueue<T, Capacity>::push(const T &value) {
    return emplace<const T &>(value);
}

#define WRITE 1u
#define READ  2u
#define DESTROY 4u
#define LAP_SHIFT 5u
#define LAP (1u << LAP_SHIFT)
#define LAP_MASK (LAP - 1u)
#define BLOCK_CAP LAP_MASK
#define SHIFT 1u
#define HAS_NEXT 1u

template<class T>
class SegQueue {
    struct Slot {
        std::aligned_storage_t<sizeof(T), alignof(T)> _value;
        std::atomic_size_t _state{0};

        void wait_write();
    };

    struct Block {
        std::atomic<Block *> _next{nullptr};
        Slot _slots[BLOCK_CAP];

        Block *wait_next();

        static void destroy(Block *self, size_t start);
    };

    struct Position {
        std::atomic_size_t _index{0};
        std::atomic<Block *> _block{nullptr};
    };

    CachePadded<Position> _head{}, _tail{};
public:
    ~SegQueue();

    template<typename ...Args>
    void emplace(Args &&...args);

    void push(const T &t);

    optional<T> pop();

    bool is_empty() const noexcept;

    size_t size() const noexcept;
};

template<class T>
SegQueue<T>::~SegQueue() {
    auto head = _head._index.load(std::memory_order_relaxed);
    auto tail = _tail._index.load(std::memory_order_relaxed);
    auto block = _head._block.load(std::memory_order_relaxed);

    head &= ~((1u << SHIFT) - 1u);
    tail &= ~((1u << SHIFT) - 1u);

    while (head != tail) {
        auto offset = (head >> SHIFT) & LAP_MASK;
        if (offset < BLOCK_CAP) {
            auto &slot = block->_slots[offset];
            ((T *) (&slot._value))->~T();
        } else {
            auto next = block->_next.load(std::memory_order_relaxed);
            delete (block);
            block = next;
        }

        head = head + (1u << SHIFT);
    }

    if (block) delete (block);
}

template<class T>
template<typename... Args>
void SegQueue<T>::emplace(Args &&... args) {
    auto back_off = BackOff();
    auto tail = _tail._index.load(std::memory_order_acquire);
    auto block = _tail._block.load(std::memory_order_acquire);
    std::unique_ptr<Block> next_block = nullptr;

    while (true) {
        auto offset = (tail >> SHIFT) & LAP_MASK;
        if (offset == BLOCK_CAP) {
            back_off.snooze();
            tail = _tail._index.load(std::memory_order_acquire);
            block = _tail._block.load(std::memory_order_acquire);
            continue;
        }
        if (offset + 1 == BLOCK_CAP && !next_block) {
            next_block = std::make_unique<Block>();
        }

        if (!block) {
            auto _new = ::new Block;
            if (_tail._block.compare_exchange_strong(block, _new, std::memory_order_release)) {
                _head._block.store(_new, std::memory_order_release);
                block = _new;
            } else {
                next_block = std::unique_ptr<Block>(_new);
                tail = _tail._index.load(std::memory_order_acquire);
                block = _tail._block.load(std::memory_order_acquire);
                continue;
            }
        }

        auto new_tail = tail + (1u << SHIFT);

        if (_tail._index.compare_exchange_weak(tail, new_tail, std::memory_order_seq_cst, std::memory_order_acquire)) {
            if (offset + 1 == BLOCK_CAP) {
                auto _next_block = next_block.release();
                auto _next_index = new_tail + (1u << SHIFT);
                _tail._block.store(_next_block, std::memory_order_release);
                _tail._index.store(_next_index, std::memory_order_release);
                block->_next.store(_next_block, std::memory_order_release);
            }

            auto &slot = block->_slots[offset];
            ::new(&slot._value) T(std::forward<Args>(args)...);
            slot._state.fetch_or(WRITE, std::memory_order_release);
            return;
        } else {
            tail = _tail._index.load(std::memory_order_acquire);
            block = _tail._block.load(std::memory_order_acquire);
            back_off.spin();
        }
    }
}

template<class T>
void SegQueue<T>::push(const T &t) {
    emplace<const T &>(t);
}

template<class T>
optional<T> SegQueue<T>::pop() {
    auto back_off = BackOff();
    auto head = _head._index.load(std::memory_order_acquire);
    auto block = _head._block.load(std::memory_order_acquire);
    while (true) {
        auto offset = (head >> SHIFT) & LAP_MASK;
        if (offset == BLOCK_CAP) {
            back_off.snooze();
            head = _head._index.load(std::memory_order_acquire);
            block = _head._block.load(std::memory_order_acquire);
            continue;
        }

        auto new_head = head + (1u << SHIFT);

        if ((new_head & HAS_NEXT) == 0) {
            std::atomic_thread_fence(std::memory_order_seq_cst);
            auto tail = _tail._index.load(std::memory_order_relaxed);
            if ((head >> SHIFT) == (tail >> SHIFT)) {
                return _opt::nullopt;
            }

            if (((head >> SHIFT) >> LAP_SHIFT) != ((tail >> SHIFT) >> LAP_SHIFT)) {
                new_head |= HAS_NEXT;
            }
        }

        if (!block) {
            back_off.snooze();
            head = _head._index.load(std::memory_order_acquire);
            block = _head._block.load(std::memory_order_acquire);
            continue;
        }

        if (_head._index.compare_exchange_weak(head, new_head, std::memory_order_seq_cst, std::memory_order_acquire)) {
            if ((offset + 1) == BLOCK_CAP) {
                auto next = block->wait_next();
                auto next_index = (new_head & ~HAS_NEXT) + (1u << SHIFT);
                if (next->_next.load(std::memory_order_relaxed)) {
                    next_index |= HAS_NEXT;
                }
                _head._block.store(next, std::memory_order_release);
                _head._index.store(next_index, std::memory_order_release);
            }

            auto &slot = block->_slots[offset];
            slot.wait_write();
            T m = std::move(*((T *) &slot._value));
            ((T *) &slot._value)->~T();
            if ((offset + 1) == BLOCK_CAP) {
                Block::destroy(block, 0);
            } else if ((slot._state.fetch_or(READ, std::memory_order_acquire) & DESTROY) != 0) {
                Block::destroy(block, offset + 1);
            }

            return {std::move(m)};
        } else {
            head = _head._index.load(std::memory_order_acquire);
            block = _head._block.load(std::memory_order_acquire);
            back_off.spin();
        }
    }
}

template<class T>
bool SegQueue<T>::is_empty() const noexcept {
    auto head = _head._index.load(std::memory_order_seq_cst);
    auto tail = _tail._index.load(std::memory_order_seq_cst);
    return (head >> SHIFT) == (tail >> SHIFT);
}

template<class T>
size_t SegQueue<T>::size() const noexcept {
    while (true) {
        auto head = _head._index.load(std::memory_order_seq_cst);
        auto tail = _tail._index.load(std::memory_order_seq_cst);
        if (_tail._index.load(std::memory_order_seq_cst) == tail) {
            tail &= ~((1u << SHIFT) - 1u);
            head &= ~((1u << SHIFT) - 1u);
            auto lap = ((head) >> SHIFT) >> LAP_SHIFT;
            tail -= (lap * LAP) << SHIFT;
            head -= (lap * LAP) << SHIFT;
            tail >>= SHIFT;
            head >>= SHIFT;
            if (head == BLOCK_CAP) {
                head = 0;
                tail -= LAP;
            }
            if (tail == BLOCK_CAP) {
                tail += 1;
            }
            return tail - head - (tail >> LAP_SHIFT);
        }
    }
}

template<class T>
typename SegQueue<T>::Block *SegQueue<T>::Block::wait_next() {
    auto backoff = BackOff();
    while (true) {
        auto next = _next.load(std::memory_order_acquire);
        if (next) {
            return next;
        }
        backoff.snooze();
    }
}

template<class T>
void SegQueue<T>::Block::destroy(SegQueue::Block *self, size_t start) {
    for (auto i = start; i < BLOCK_CAP - 1; i++) {
        auto &slot = self->_slots[i];
        if ((slot._state.load(std::memory_order_acquire) & READ) == 0 &&
            (slot._state.fetch_or(DESTROY, std::memory_order_acquire) & READ) == 0)
            return;
    }
    delete (self);
}

template<class T>
void SegQueue<T>::Slot::wait_write() {
    auto back_off = BackOff();
    while ((_state.load(std::memory_order_acquire) & WRITE) == 0) {
        back_off.snooze();
    }
}


#undef WRITE
#undef READ
#undef DESTROY
#undef LAP
#undef BLOCK_CAP
#undef SHIFT
#undef HAS_NEXT
#endif //STANFORD_QUEUE_H
