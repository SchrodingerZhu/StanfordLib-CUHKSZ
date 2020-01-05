//
// Created by schrodinger on 12/11/19.
//

#ifndef STANFORD_TEST_HEADER_H
#define STANFORD_TEST_HEADER_H
#include <util/random.h>
#include <QtTest/QtTest>

#ifndef TESTLIB_SELFCOVERAGE_START
#define TESTLIB_SELFCOVERAGE_START(name)
#endif

#define LIB_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    TESTLIB_SELFCOVERAGE_START(#TestObject) \
    stanfordcpplib::initializeLibrary(argc, argv); \
    QTEST_DISABLE_KEYPAD_NAVIGATION \
    TestObject tc; \
    QTEST_SET_MAIN_SOURCE_PATH \
    auto code = QTest::qExec(&tc, argc, argv); \
    stanfordcpplib::shutdownLibrary();\
    return code; \
}

inline auto random_string() {
    std::string test{};
    auto len = randomInteger(0, 1000);
    while(len--) {
        test.push_back(static_cast<char>(randomInteger(32, 126)));
    }
    return test;
}

#endif //STANFORD_TEST_HEADER_H
