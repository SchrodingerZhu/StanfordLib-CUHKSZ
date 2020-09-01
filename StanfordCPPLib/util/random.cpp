/*
 * File: random.cpp
 * ----------------
 * This file implements the random.h interface.
 * 
 * @version 2019/05/16
 * - added randomColor that takes min/max RGB
 * @version 2017/10/05
 * - added randomFeedClear
 * @version 2017/09/28
 * - moved random 'feed' functions into autograder namespace
 * - ensure that randomly fed integers are within the specified range
 * @version 2016/10/04
 * - removed all static variables (replaced with STATIC_VARIABLE macros)
 * @version 2016/08/02
 * - added randomColor, randomColorString
 * @version 2014/10/19
 * - alphabetized functions
 * @version 2014/10/08
 * - removed 'using namespace' statement
 */

#include <util/random.h>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <queue>
#include <system/error.h>
#include "private/static.h"
#include <sstream>

/* internal buffer of fixed random numbers to return; used by autograders */
STATIC_VARIABLE_DECLARE_COLLECTION_EMPTY(std::queue<bool>, fixedBools)

STATIC_VARIABLE_DECLARE_COLLECTION_EMPTY(std::queue<int>, fixedInts)

STATIC_VARIABLE_DECLARE_COLLECTION_EMPTY(std::queue<double>, fixedReals)

namespace autograder {
    void randomFeedBool(bool value) {
        STATIC_VARIABLE(fixedBools).push(value);
    }

    void randomFeedClear() {
        STATIC_VARIABLE(fixedBools) = std::queue<bool>();
        STATIC_VARIABLE(fixedInts) = std::queue<int>();
        STATIC_VARIABLE(fixedReals) = std::queue<double>();
    }

    void randomFeedInteger(int value) {
        STATIC_VARIABLE(fixedInts).push(value);
    }

    void randomFeedReal(double value) {
        STATIC_VARIABLE(fixedReals).push(value);
    }
}

/* end 'fixed' internal stuff */

bool randomBool() {
    return randomChance(0.5);
}

/*
 * Implementation notes: randomChance
 * ----------------------------------
 * The code for randomChance calls randomReal(0, 1) and then checks
 * whether the result is less than the requested probability.
 */
bool randomChance(double p) {
    if (!STATIC_VARIABLE(fixedBools).empty()) {
        bool top = STATIC_VARIABLE(fixedBools).front();
        STATIC_VARIABLE(fixedBools).pop();
        return top;
    }
    return randomReal(0, 1) < p;
}

unsigned randomColor() {
    if (!STATIC_VARIABLE(fixedInts).empty()) {
        unsigned top = STATIC_VARIABLE(fixedInts).front();
        STATIC_VARIABLE(fixedInts).pop();
        return top & 0x00ffffffu;
    }
    return static_cast<unsigned>(randomInteger(0, RAND_MAX)) & 0x00ffffffu;
}

unsigned randomColor(unsigned minRGB, unsigned maxRGB) {
    if (!STATIC_VARIABLE(fixedInts).empty()) {
        return randomColor();
    }
    if (minRGB > 255 || maxRGB > 255
        || minRGB > maxRGB) {
        error("randomColor: min/max values out of range");
    }
    unsigned r = randomInteger(static_cast<int>(minRGB), static_cast<int>(maxRGB));
    unsigned g = randomInteger(static_cast<int>(minRGB), static_cast<int>(maxRGB));
    unsigned b = randomInteger(static_cast<int>(minRGB), static_cast<int>(maxRGB));
    return r << 16u | g << 8u | b;
}

// see convertRGBToColor in gcolor.h (repeated here to avoid Qt dependency)
std::string randomColorString() {
    unsigned rgb = randomColor();
    std::ostringstream os;
    os << std::hex << std::uppercase << "#";
    os << std::setw(2) << std::setfill('0') << (rgb >> 16u & 0xFFu);
    os << std::setw(2) << std::setfill('0') << (rgb >> 8u & 0xFFu);
    os << std::setw(2) << std::setfill('0') << (rgb & 0xFFu);
    return os.str();
}

std::string randomColorString(unsigned minRGB, unsigned maxRGB) {
    unsigned rgb = randomColor(minRGB, maxRGB);
    std::ostringstream os;
    os << std::hex << std::uppercase << "#";
    os << std::setw(2) << std::setfill('0') << (rgb >> 16u & 0xFFu);
    os << std::setw(2) << std::setfill('0') << (rgb >> 8u & 0xFFu);
    os << std::setw(2) << std::setfill('0') << (rgb & 0xFFu);
    return os.str();
}

/*
 * Implementation notes: randomInteger
 * -----------------------------------
 * The code for randomInteger produces the number in four steps:
 *
 * 1. Generate a random real number d in the range [0 .. 1).
 * 2. Scale the number to the range [0 .. N) where N is the number of values.
 * 3. Translate the number so that the range starts at the appropriate value.
 * 4. Convert the result to the next lower integer.
 *
 * The implementation is complicated by the fact that both the expression
 *
 *     RAND_MAX + 1
 *
 * and the expression for the number of values
 *
 *     high - low + 1
 *
 * can overflow the integer range.  These calculations must therefore be
 * performed using doubles instead of ints.
 */
int randomInteger(int low, int high) {
    if (!STATIC_VARIABLE(fixedInts).empty()) {
        int top = STATIC_VARIABLE(fixedInts).front();
        STATIC_VARIABLE(fixedInts).pop();
        if (top < low || top > high) {
            // make sure the value is in the given range
            // (assumes that low/high don't overflow int range)
            int range = high - low + 1;
            top = low + std::abs(top) % range;
        }
        return top;
    }
    double d = __rng();
    double s = d * (double(high) - low + 1);
    return int(floor(low + s));
}

/*
 * Implementation notes: randomReal
 * --------------------------------
 * The code for randomReal is similar to that for randomInteger,
 * without the final conversion step.
 */
double randomReal(double low, double high) {
    if (!STATIC_VARIABLE(fixedReals).empty()) {
        double top = STATIC_VARIABLE(fixedReals).front();
        STATIC_VARIABLE(fixedReals).pop();
        return top;
    }
    double d = __rng();
    double s = d * (high - low);
    return low + s;
}

/*
 * Implementation notes: setRandomSeed
 * -----------------------------------
 * The setRandomSeed function simply forwards its argument to srand.
 * The call to initRandomSeed is required to set the initialized flag.
 */
void setRandomSeed(int seed) {
    __rng.rngEng.seed(seed);
}

__RNG__::__RNG__() noexcept : rngDev(), rngEng(rngDev()), dist(0, 1) {}

double __RNG__::operator()() {
    return dist(rngEng);
}
