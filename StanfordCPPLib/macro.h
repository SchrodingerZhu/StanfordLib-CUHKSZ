#ifndef __APPLE__

#include <mimalloc-override.h>

#endif

#include "private/init.h"
#include <graphics/qtgui.h>
#include <console/console.h>

#define run(argc, argv, studentMain) \
    stanfordcpplib::initializeLibrary(argc, argv); \
    QtGui::instance()->startBackgroundEventLoop(studentMain); \
    stanfordcpplib::shutdownLibrary(); \
    return 0; \

#define __WRAP_MAIN__(start) int main(int argc, char** argv) {\
    run(argc, argv, start)\
}

