//
// Created by schrodinger on 1/15/20.
//

#ifndef STANFORD_MUTEX_H
#define STANFORD_MUTEX_H

#include <mutex>
#include <QMutex>
#include <sync/lock_api.h>
#include <absl/synchronization/mutex.h>

class AbslLockWrapper {
    std::unique_ptr<absl::Mutex> mutex;
public:
    AbslLockWrapper() : mutex(std::make_unique<absl::Mutex>()) {};

    void lock() {
        mutex->Lock();
    }

    void unlock() {
        mutex->Unlock();
    }
};


template<typename T>
using SMutex = lock_api::Mutex<T, std::mutex>;

template<typename T>
using QtMutex = lock_api::Mutex<T, QMutex>;

template<typename T>
using AMutex = lock_api::Mutex<T, AbslLockWrapper>;
#endif //STANFORD_MUTEX_H
