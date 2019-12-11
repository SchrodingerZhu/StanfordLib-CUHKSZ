//
// Created by schrodinger on 12/11/19.
//

#ifndef STANFORD_TEST_HEADER_H
#include <QtTest/QtTest>
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
#define STANFORD_TEST_HEADER_H

#endif //STANFORD_TEST_HEADER_H
