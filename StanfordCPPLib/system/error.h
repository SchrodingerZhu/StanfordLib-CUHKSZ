/*
 * File: error.h
 * -------------
 * This file defines the <code>ErrorException</code> class and the
 * <code>error</code> function.
 *
 * @version 2018/10/18
 * - added getKind for ErrorExceptions that wrap other types of errors
 * @version 2018/09/25
 * - added doc comments for new documentation generation
 * @version 2017/11/29
 * - fix for undefined SIGUSR1 on Windows systems
 * @version 2016/11/29
 * - changed error() to accept const string& instead of string
 * @version 2016/11/23
 * - added operator << to print ErrorExceptions
 */


#ifndef _error_h
#define _error_h
#define _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES
#include <csignal>
#include <exception>
#include <iostream>
#include <string>

// bug fix for missing SIGUSR1 on some Windows systems
#ifndef SIGUSR1
#define SIGUSR1 10
#endif // SIGUSR2

/**
 * This exception is thrown by calls to the <code>error</code>
 * function.  Typical code for catching errors looks like this:
 *
 *<pre>
 *    try {
 *       ... code in which an error might occur ...
 *    } catch (ErrorException& ex) {
 *       ... code to handle the error condition ...
 *    }
 *</pre>
 *
 * If an <code>ErrorException</code> is thrown at any point in the
 * range of the <code>try</code> (including in functions called from
 * that code), control will jump immediately to the error handler.
 */
class ErrorException : public std::exception {
public:
    /**
     * Creates a new ErrorException with the given error message.
     */
    explicit ErrorException(std::string msg, std::string _kind = "error");
    ErrorException(const ErrorException& errorException) noexcept(false) = default;
    /**
     * Frees any memory allocated by the exception.
     */
    ~ErrorException() noexcept override = default;

    /**
     * Prints the exception to the standard error stream (cerr),
     * including its message and stack trace if any.
     */
    virtual void dump() const;

    /**
     * Prints the exception to the given output stream,
     * including its message and stack trace if any.
     */
    virtual void dump(std::ostream &out) const;

    /**
     * Returns what kind of exception this is.
     * In general this returns "error", but in some cases we catch other kinds
     * of exceptions (like thrown ints or strings) and wrap them up as
     * ErrorExceptions. In such cases, the kind will be "int" or "string" etc.
     */
    [[nodiscard]] virtual std::string getKind() const;

    /**
     * Returns the exception's error message as passed to its constructor.
     */
    [[nodiscard]] virtual std::string getMessage() const;

    /**
     * Returns a stack trace for this exception as a multi-line string.
     * See exceptions.h/cpp for descriptions of the format.
     * Not every exception has a proper stack trace, based on when/why it was
     * thrown, platform incompatibilities, and other issues; use hasStackTrace to
     * check if a given exception's stack trace is populated.
     */
    [[nodiscard]] virtual std::string getStackTrace() const;

    /**
     * Returns whether this exception has a non-empty stack trace.
     * Not every exception has a proper stack trace, based on when/why it was
     * thrown, platform incompatibilities, and other issues; use hasStackTrace to
     * check if a given exception's stack trace is populated.
     */
    [[nodiscard]] virtual bool hasStackTrace() const;

    /**
     * Sets what kind of exception this is.
     * Default is "error".
     */
    void setKind(const std::string &kind);

    /**
     * Returns the exception's error message as a C string.
     */
    [[nodiscard]] const char *what() const noexcept override;

protected:
    /**
     * Sets this exception's stack trace to the given multi-line string.
     */
    void setStackTrace(const std::string &stackTrace);

private:
    std::string _kind;
    std::string _msg;
    std::string _stackTrace;

    /**
     * Prepends "*** " to each line of the given string.
     * Used to format stack traces that print to the console.
     */
    static std::string insertStarsBeforeEachLine(const std::string &s);
};

/**
 * Prints the error exception to the given output stream.
 */
std::ostream &operator<<(std::ostream &out, const ErrorException &ex);

/**
 * Signals an error condition in a program by throwing an
 * <code>ErrorException</code> with the specified message.
 */
[[noreturn]] void error(const std::string &msg);

#undef _LIBCPP_ENABLE_CXX17_REMOVED_FEATURES
#endif // _error_h
