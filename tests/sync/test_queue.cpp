//
// Created by schrodinger on 1/15/20.
//

#include "../test_header.h"
#include <macro.h>
#include <algorithm>
#include <sync/queue.h>

class TestQueue : public QObject {
Q_OBJECT
private slots:

    static void test_array_queue();
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

QTEST_GUILESS_MAIN(TestQueue)

#include "test_queue.moc"