#include "../test_header.h"
#include <util/strlib.h>
#include <private/init.h>
#include "../test_header.h"
#define TEST_SIZE 100'000
class TestVector: public QObject
{
Q_OBJECT
private slots:
    static void boolBasic();
    static void integerBasic();
    static void trivialRandom();
};

void TestVector::boolBasic() {
    Vector<bool> a(5, true);
    QCOMPARE(a, Vector<bool> ({true, true, true, true, true}));
    a.emplace(false);
    a.push_back(false);
    a.push_front(false);
    QCOMPARE(a, Vector<bool> ({false, true, true, true, true, true, false, false}));
    auto b = a;
    while(a == b) {
        a.shuffle();
    }
    QVERIFY(a != b);
    b.remove(1);
    b.removeFront();
    b.removeBack();
    QCOMPARE(b, Vector<bool> ({true, true, true, true, false}));
    QVERIFY(!b.isEmpty());
    QVERIFY(Vector<bool>().isEmpty());
    a.clear();
    QVERIFY(a.isEmpty());
}

void TestVector::integerBasic() {
    {
        Vector<int> a(5, 19991208);
        QCOMPARE(a, Vector<int> ({19991208, 19991208, 19991208, 19991208, 19991208}));
    }
    Vector<int> a = {1, 2, 3, 4, 5};
    QCOMPARE(a, Vector<int> ({1, 2, 3, 4, 5}));
    a.emplace(6);
    a.push_back(7);
    a.push_front(0);
    QCOMPARE(a, Vector<int> ({0, 1, 2, 3, 4, 5, 6, 7}));
    auto b = a;
    while(a == b) {
        a.shuffle();
    }
    QVERIFY(a != b);
    b.remove(1);
    b.removeFront();
    b.removeBack();
    QCOMPARE(b, Vector<int> ({2, 3, 4, 5, 6}));
    QVERIFY(!b.isEmpty());
    QVERIFY(Vector<int>().isEmpty());
    a.clear();
    QVERIFY(a.isEmpty());
}

void TestVector::trivialRandom() {
    {
        std::vector<double> tester {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        Vector<double> testee {1, 2, 3, 4, 5, 6, 7, 8, 9, 10.4};
        QVERIFY(!(testee == tester));
    }
    std::vector<double> tester {};
    Vector<double> testee {};

    QCOMPARE(testee, testee);
    QCOMPARE(testee, tester);
    for (int i = 0; i < TEST_SIZE; ++i) {
        auto a = randomReal(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
        tester.push_back(a);
        testee.push_back(a);
    }
    QCOMPARE(testee, tester);
}


LIB_TEST_MAIN(TestVector)
#include "test_vector.moc"

#undef TEST_SIZE
#undef USE_SYNC_POOL_RESOURCE
