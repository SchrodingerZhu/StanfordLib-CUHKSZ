//
// Created by schrodinger on 12/11/19.
//

#ifndef STANFORD_TEST_HEADER_H

#include <memory_resource>
#ifdef USE_SYNC_POOL_RESOURCE
#define INIT_MEM_RESOURCE \
    std::pmr::synchronized_pool_resource __GLOBAL__; \
    std::pmr::set_default_resource(&__GLOBAL__);
#elif defined(USE_UNSYNC_POOL_RESOURCE)
#define INIT_MEM_RESOURCE \
    std::pmr::unsynchronized_pool_resource __GLOBAL__; \
    std::pmr::set_default_resource(&__GLOBAL__);
#else
#define INIT_MEM_RESOURCE
#endif

#include <QtTest/QtTest>
#define LIB_TEST_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    INIT_MEM_RESOURCE \
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
