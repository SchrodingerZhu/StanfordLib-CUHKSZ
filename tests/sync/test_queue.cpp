//
// Created by schrodinger on 1/15/20.
//

#include "../test_header.h"
#include <macro.h>
#include <algorithm>
#include <sync/queue.h>
#include <util/random.h>

#define TEST_SIZE 5000

class TestQueue : public QObject {
Q_OBJECT
private slots:

    static void test_array_queue();

    static void test_array_queue_large();

    static void test_seg_queue();

    static void test_seg_queue_large();
};

void TestQueue::test_array_queue() {
    ArrayQueue<int, 5> q;
    std::vector<int> res;
    std::vector<int> exp = {0, 1, 2, 3, 4};
    {
        std::atomic_bool flag{true};
#pragma omp parallel for default(none) shared(q, flag)
        for (int i = 0; i < 5; ++i) {
            flag = flag & !q.pop();
        }
        QVERIFY(flag);
    }
    {
        std::atomic_bool flag{true};
#pragma omp parallel for default(none) shared(q, flag)
        for (int i = 0; i < 5; ++i) {
            flag = flag & q.push(i);
        }
        QVERIFY(flag);
    }

    QCOMPARE(q.size(), 5);

    {
        std::atomic_bool flag{true};
#pragma omp parallel for default(none) shared(q, flag)
        for (int i = 0; i < 5; ++i) {
            flag = flag & !q.push(i);
        }
        QVERIFY(flag);
    }


#pragma omp parallel for default(none) shared(q, res)
    for (int i = 0; i < 5; ++i) {
        auto p = q.pop();
#pragma omp critical
        res.push_back(p.value());
    }


    {
        std::atomic_bool flag{true};
#pragma omp parallel for default(none) shared(q, flag)
        for (int i = 0; i < 5; ++i) {
            flag = flag & !q.pop();
        }
        QVERIFY(flag);
    }

    QCOMPARE(q.size(), 0);

    std::sort(res.begin(), res.end());
    QCOMPARE(res, exp);
}

void TestQueue::test_seg_queue() {
    SegQueue<int> q;
    std::vector<int> res;
    std::vector<int> exp = {0, 1, 2, 3, 4};
    QCOMPARE(q.size(), 0);
    {
        std::atomic_bool flag{true};
#pragma omp parallel for default(none) shared(q, flag)
        for (int i = 0; i < 5; ++i) {
            flag = flag & !q.pop();
        }
        QVERIFY(flag);
    }
    {
#pragma omp parallel for default(none) shared(q)
        for (int i = 0; i < 5; ++i) {
            q.push(i);
        }
    }


#pragma omp parallel for default(none) shared(q, res)
    for (int i = 0; i < 2; ++i) {
        auto p = q.pop();
#pragma omp critical
        res.push_back(p.value());
    }
    QCOMPARE(q.size(), 3);
#pragma omp parallel for default(none) shared(q, res)
    for (int i = 0; i < 3; ++i) {
        auto p = q.pop();
#pragma omp critical
        res.push_back(p.value());
    }

    QCOMPARE(q.size(), 0);

    {
        std::atomic_bool flag{true};
#pragma omp parallel for default(none) shared(q, flag)
        for (int i = 0; i < 5; ++i) {
            flag = flag & !q.pop();
        }
        QVERIFY(flag);
    }

    QCOMPARE(q.size(), 0);

    std::sort(res.begin(), res.end());
    QCOMPARE(res, exp);
}

void TestQueue::test_array_queue_large() {
    ArrayQueue<std::string, TEST_SIZE> q;
    std::vector<std::string> a, b;
    for (int i = 0; i < TEST_SIZE; ++i) {
        a.push_back(random_string());
    }
#pragma omp parallel for default(none) shared(q, a)
    for (auto i = a.begin(); i < a.end(); ++i) {
        q.push(*i);
    }
    QCOMPARE(q.size(), TEST_SIZE);
#pragma omp parallel for default(none) shared(q, b)
    for (auto i = 0; i < TEST_SIZE; ++i) {
        auto m = q.pop();
#pragma omp critical
        b.push_back(m.value());
    }
    QCOMPARE(q.size(), 0);
    std::sort(b.begin(), b.end());
    std::sort(a.begin(), a.end());
    QCOMPARE(b, a);
}

void TestQueue::test_seg_queue_large() {
    SegQueue<std::string> q;
    std::vector<std::string> a, b;
    for (int i = 0; i < TEST_SIZE; ++i) {
        a.push_back(random_string());
    }
#pragma omp parallel for default(none) shared(q, a)
    for (auto i = a.begin(); i < a.end(); ++i) {
        q.push(*i);
    }
    QCOMPARE(q.size(), TEST_SIZE);
#pragma omp parallel for default(none) shared(q, b)
    for (auto i = 0; i < TEST_SIZE; ++i) {
        auto m = q.pop();
#pragma omp critical
        b.push_back(m.value());
    }
//    QCOMPARE(q.size(), 0);
//    std::sort(b.begin(), b.end());
//    std::sort(a.begin(), a.end());
//    QCOMPARE(b, a);
}

QTEST_GUILESS_MAIN(TestQueue)

#include "test_queue.moc"