#include "private/init.h"
#include <graphics/qtgui.h>
#include <console/console.h>
#if __cplusplus >= 201703L
#include <memory_resource>
#endif

#define run(argc, argv, studentMain) \
    stanfordcpplib::initializeLibrary(argc, argv); \
    QtGui::instance()->startBackgroundEventLoop(studentMain); \
    stanfordcpplib::shutdownLibrary(); \
    return 0; \

#define __WRAP_MAIN__(start) int main(int argc, char** argv) {\
    run(argc, argv, start)\
}

#if __cplusplus >= 201703L
#define __WRAP_MAIN_SYNC_POOL__(start) int main(int argc, char** argv) {\
    std::pmr::synchronized_pool_resource __GLOBAL__; \
    std::pmr::set_default_resource(&__GLOBAL__); \
    run(argc, argv, start)\
}

#define __WRAP_MAIN_UNSYNC_POOL__(start) int main(int argc, char** argv) {\
    std::pmr::unsynchronized_pool_resource __GLOBAL__; \
    std::pmr::set_default_resource(&__GLOBAL__); \
    run(argc, argv, start)\
}
#endif
