//
// Created by schrodinger on 1/15/20.
//

#include "../test_header.h"
#include <macro.h>
#include <sync/mutex.h>
#include <sync/rw_lock.h>

class TestMutex : public QObject {
Q_OBJECT

    template<template<class> typename U>
    static void test_lock();

private slots:

    static void test_smutex();

    static void test_qtmutex();

    static void test_amutex();

    static void test_move();

    static void test_rwlock();
};

void TestMutex::test_smutex() {
    test_lock<SMutex>();
}

template<template<class> typename U>
void TestMutex::test_lock() {
    for (int i = 0; i < 5; ++i) {
        U<long> t(0);
#pragma omp parallel for default(none) shared(t)
        for (long int i = 0; i < 1000; ++i) {
            auto g = t.lock();
            *g += 1;
        }
        auto g = t.lock();
        QCOMPARE(*g, 1000);
    }
}

void TestMutex::test_qtmutex() {
    test_lock<QtMutex>();
}

void TestMutex::test_amutex() {
    test_lock<AMutex>();
}

void TestMutex::test_move() {
    std::vector<std::string> a{"a", "b", "c"}, b{"a", "b", "c"};
    AMutex<std::vector<std::string>> t(std::move(a));
    auto x = std::move(t);
    std::atomic_bool flag = true;
#pragma omp parallel for default(none) shared(x, b, flag)
    for (int i = 0; i < 1000; ++i) {
        auto y = x.lock();
        flag = flag & (*y == b);
    }
    QVERIFY(flag);
}

void TestMutex::test_rwlock() {
    for (int i = 0; i < 5; ++i) {
        RwLock<long> t(0);
#pragma omp parallel for default(none) shared(t)
        for (long int i = 0; i < 1000; ++i) {
            auto h = t.lockForRead();
            h.drop();
            auto g = t.lockForWrite();
            *g += 1;
        }
        auto g = t.lockForRead();
        QCOMPARE(*g, 1000);
    }
}

LIB_TEST_MAIN(TestMutex)

#include "test_locks.moc"