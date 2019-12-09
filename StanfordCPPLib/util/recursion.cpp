/*
 * File: recursion.cpp
 * -------------------
 * This file implements the recursion.h interface.
 */

#include <util/recursion.h>
#include <system/exceptions.h>
#include <system/call_stack.h>

int getRecursionIndentLevel() {
    // constructing the following object jumps into fancy code in call_stack_gcc/windows.cpp
    // to rebuild the stack trace; implementation differs for each operating system
    stacktrace::call_stack trace;
    Vector<stacktrace::entry> entries = trace.stack;

    std::string currentFunction = {};
    int currentFunctionCount = 0;
    for (auto & entrie : entries) {
        // remove references to std:: namespace
        if (exceptions::shouldFilterOutFromStackTrace(entrie.function)
            || entrie.function.find("recursionIndent(") != std::string::npos
            || entrie.function.find("getRecursionIndentLevel(") != std::string::npos) {
            continue;
        } else if (currentFunction.empty()) {
            currentFunction = entrie.function;
            currentFunctionCount = 1;
        } else if (entrie.function == currentFunction) {
            currentFunctionCount++;
        } else {
            break;
        }
    }
    return currentFunctionCount;
}

std::string recursionIndent(const std::string &indenter) {
    int indent = getRecursionIndentLevel();
    std::string result = {};
    for (int i = 0; i < indent - 1; i++) {
        result += indenter;
    }
    return result;
}
