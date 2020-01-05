#include "private/init.h"
#include <graphics/qtgui.h>
#include <console/console.h>

#ifndef __OVERRIDE_MIMALLOC__
#define __OVERRIDE_MIMALLOC__

#include <mimalloc-new-delete.h>

#endif

#define run(argc, argv, studentMain) \
    stanfordcpplib::initializeLibrary(argc, argv); \
    QtGui::instance()->startBackgroundEventLoop(studentMain); \
    stanfordcpplib::shutdownLibrary(); \
    return 0; \

#define __WRAP_MAIN__(start) int main(int argc, char** argv) {\
    run(argc, argv, start)\
}

