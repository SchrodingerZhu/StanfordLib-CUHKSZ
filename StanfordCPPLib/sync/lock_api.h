//
// Created by schrodinger on 1/15/20.
//

#ifndef STANFORDLIB_CUHKSZ_LOCK_API_H
#define STANFORDLIB_CUHKSZ_LOCK_API_H

#include <private/compiler.h>
#include <exception>
#include <stdexcept>
#include <memory>
#include <absl/synchronization/test_locks.h>

namespace lock_api {
    template<typename T, typename RawLock>
    class Mutex;

    template<typename T, typename RawLock>
    class RwLock;

    template<typename T, typename RawLock>
    class LockGuard {
        RawLock *_inner = nullptr;
        T *content = nullptr;

        LockGuard(RawLock *_inner, T *content);

    public:
        T *operator->() const;

        T &operator*() const;

        void drop();

        ~LockGuard();

        LockGuard(LockGuard &&that) noexcept;

        LockGuard &operator=(LockGuard &&that) noexcept;

        LockGuard(const LockGuard &that) = delete;

        LockGuard &operator=(const LockGuard &that) = delete;

        friend Mutex<T, RawLock>;
        friend RwLock<T, RawLock>;
    };

    template<typename T, typename RawLock>
    T *LockGuard<T, RawLock>::operator->() const {
        if (unlikely(_inner == nullptr || content == nullptr)) {
            throw std::runtime_error("try to get value from a imcomplete lock");
        }
        return content;
    }

    template<typename T, typename RawLock>
    T &LockGuard<T, RawLock>::operator*() const {
        if (unlikely(_inner == nullptr || content == nullptr)) {
            throw std::runtime_error("try to get value from a imcomplete lock");
        }
        return *content;
    }

    template<typename T, typename RawLock>
    void LockGuard<T, RawLock>::drop() {
        if (unlikely(_inner == nullptr || content == nullptr)) {
            throw std::runtime_error("try to drop an imcomplete lock");
        }
        _inner->unlock();
        _inner = nullptr;
        content = nullptr;
    }

    template<typename T, typename RawLock>
    LockGuard<T, RawLock>::~LockGuard() {
        if (_inner && content) {
            _inner->unlock();
            _inner = nullptr;
            content = nullptr;
        }
    }

    template<typename T, typename RawLock>
    LockGuard<T, RawLock>::LockGuard(RawLock *_inner, T *content) : _inner(_inner), content(content) {
        if (unlikely(_inner == nullptr || content == nullptr)) {
            throw std::runtime_error("invalid data for creating a lock guard");
        }
    }

    template<typename T, typename RawLock>
    LockGuard<T, RawLock>::LockGuard(LockGuard &&that) noexcept : _inner(that._inner), content(that.content) {
        that._inner = nullptr;
        that.content = nullptr;
    }

    template<typename T, typename RawLock>
    LockGuard<T, RawLock> &LockGuard<T, RawLock>::operator=(LockGuard &&that) noexcept {
        if (_inner && content) drop();
        _inner = that._inner;
        content = that.content;
        that._inner = nullptr;
        that.content = nullptr;
        return *this;
    }

    template<typename T, typename RawLock>
    class Mutex {
        RawLock _inner{};
        std::unique_ptr<T> content;
    public:
        explicit Mutex(const T &content);

        template<typename ...Args>
        explicit Mutex(Args &&... args);

        LockGuard<T, RawLock> lock();

        ~Mutex() = default;

        Mutex(Mutex &&that) noexcept;

        Mutex &operator=(Mutex &&that) noexcept;

        Mutex(const Mutex &that) = delete;

        Mutex &operator=(const Mutex &that) = delete;
    };

    template<typename T, typename RawLock>
    Mutex<T, RawLock>::Mutex(const T &content) : content(std::make_unique<T>(content)) {}

    template<typename T, typename RawLock>
    template<typename... Args>
    Mutex<T, RawLock>::Mutex(Args &&... args) : content(std::make_unique<T>(std::forward<Args>(args)...)) {}

    template<typename T, typename RawLock>
    Mutex<T, RawLock>::Mutex(Mutex &&that) noexcept : _inner(std::move(that._inner)),
                                                      content(std::move(that.content)) {}

    template<typename T, typename RawLock>
    Mutex<T, RawLock> &Mutex<T, RawLock>::operator=(Mutex &&that) noexcept {
        _inner = std::move(that._inner);
        content = std::move(that.content);
    }

    template<typename T, typename RawLock>
    LockGuard<T, RawLock> Mutex<T, RawLock>::lock() {
        _inner.lock();
        return {&_inner, content.get()};
    }

    template<typename T, typename RawLock>
    class RwLock {
        RawLock _inner{};
        std::unique_ptr<T> content;
    public:
        explicit RwLock(const T &content);

        template<typename ...Args>
        explicit RwLock(Args &&... args);

        LockGuard<T, RawLock> lockForWrite();

        LockGuard<T, RawLock> lockForRead();

        ~RwLock() = default;

        RwLock(RwLock &&that) noexcept;

        RwLock &operator=(RwLock &&that) noexcept;

        RwLock(const RwLock &that) = delete;

        RwLock &operator=(const RwLock &that) = delete;
    };

    template<typename T, typename RawLock>
    RwLock<T, RawLock>::RwLock(const T &content) : content(std::make_unique<T>(content)) {}

    template<typename T, typename RawLock>
    template<typename... Args>
    RwLock<T, RawLock>::RwLock(Args &&... args) : content(std::make_unique<T>(std::forward<Args>(args)...)) {}

    template<typename T, typename RawLock>
    LockGuard<T, RawLock> RwLock<T, RawLock>::lockForWrite() {
        this->_inner.lockForWrite();
        return {&this->_inner, this->content.get()};
    }

    template<typename T, typename RawLock>
    LockGuard<T, RawLock> RwLock<T, RawLock>::lockForRead() {
        this->_inner.lockForRead();
        return {&this->_inner, this->content.get()};
    }

    template<typename T, typename RawLock>
    RwLock<T, RawLock>::RwLock(RwLock &&that) noexcept : _inner(std::move(that._inner)),
                                                         content(std::move(that.content)) {}

    template<typename T, typename RawLock>
    RwLock<T, RawLock> &RwLock<T, RawLock>::operator=(RwLock &&that) noexcept {
        _inner = std::move(that._inner);
        content = std::move(that.content);
    }


}
#endif //STANFORDLIB_CUHKSZ_LOCK_API_H
