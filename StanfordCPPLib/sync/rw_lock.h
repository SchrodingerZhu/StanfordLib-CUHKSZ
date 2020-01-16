//
// Created by schrodinger on 1/15/20.
//

#ifndef STANFORD_RWLOCK_H
#define STANFORD_RWLOCK_H

#include <QReadWriteLock>
#include <sync/lock_api.h>


template<typename T>
using RwLock = lock_api::RwLock<T, QReadWriteLock>;
#endif //STANFORD_RWLOCK_H
