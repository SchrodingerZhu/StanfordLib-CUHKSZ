/*
 * File: mainwrapper.cpp
 * ---------------------
 * This file contains a 'main' function definition that initializes the 
 * Stanford C++ library, then runs the student's main function
 * in its own thread.  This is necessary for the Qt version of the library to
 * function properly.
 *
 * @version 2018/10/18
 * - multi-main initial implementation
 * @version 2018/10/07
 * - bug fixes for autograder mode
 * @version 2018/09/23
 * - bug fixes for windows Qt main wrapper
 * @version 2018/09/17
 * - initial version
 */

#include "private/init.h"
#include <graphics/qtgui.h>

#include <utility>

// student implements what looks like ordinary main() function, but

// initializes the Qt GUI library subsystems and Qt graphical console as needed
// before calling the student's main
int run(int argc, char** argv, std::function<int()> studentMain)
{
    stanfordcpplib::initializeLibrary(argc, argv);
#ifdef SPL_HEADLESS_MODE
    studentMain(); 
#else
    QtGui::instance()->startBackgroundEventLoop(std::move(studentMain));
#endif
    stanfordcpplib::shutdownLibrary();
    return 0;
}

