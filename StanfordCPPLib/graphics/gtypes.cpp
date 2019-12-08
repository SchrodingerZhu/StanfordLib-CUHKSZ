/*
 * File: gtypes.cpp
 * ----------------
 * This file implements the classes in the gtypes.h interface.
 *
 * @version 2019/05/16
 * - added GRectangle contains(GRectangle), intersects
 * @version 2018/07/14
 * - initial version, based on gtypes.cpp
 */

#include <graphics/gtypes.h>
#include <cmath>
#include <sstream>
#include <string>
#include <collections/collections.h>
#include <system/error.h>
#include <util/gmath.h>
#include <collections/hashcode.h>
#include <util/point.h>
#include <util/strlib.h>

/*
 * Implementation notes: GDimension class
 * --------------------------------------
 * The GDimension class itself is entirely straightforward.  The
 * relational operators compare the width first, followed by the height.
 * The hashCode function computes the exclusive-or of the individual words.
 */

GDimension::GDimension(double width, double height)
        : _width(width),
          _height(height) {
    // empty
}

double GDimension::getWidth() const {
    return _width;
}

double GDimension::getHeight() const {
    return _height;
}

std::string GDimension::toString() const {
    std::ostringstream out;
    out << *this;
    return out.str();
}

std::ostream &operator<<(std::ostream &os, const GDimension &dim) {
    return os << "(" << dim._width << ", " << dim._height << ")";
}

bool operator==(const GDimension &d1, const GDimension &d2) {
    return floatingPointEqual(d1._width, d2._width)
           && floatingPointEqual(d1._height, d2._height);
}

bool operator!=(const GDimension &d1, const GDimension &d2) {
    return !(d1 == d2);
}

bool operator<(const GDimension &d1, const GDimension &d2) {
    return d1._width < d2._width
           || (floatingPointEqual(d1._width, d2._width) && d1._height < d2._height);
}

bool operator<=(const GDimension &d1, const GDimension &d2) {
    return d1 < d2 || d1 == d2;
}

bool operator>(const GDimension &d1, const GDimension &d2) {
    return d2 < d1;
}

bool operator>=(const GDimension &d1, const GDimension &d2) {
    return d1 > d2 || d1 == d2;
}

GDimension operator*(const GDimension &d, double scale) {
    return GDimension(d._width * scale, d._height * scale);
}

int hashCode(const GDimension &dim) {
    return hashCode(dim._width, dim._height);
}

std::string toString(HorizontalAlignment alignment) {
    if (alignment == ALIGN_CENTER) {
        return "Center";
    } else if (alignment == ALIGN_LEFT) {
        return "Left";
    } else {   // if (alignment == ALIGN_RIGHT)
        return "Right";
    }
}

std::string toString(VerticalAlignment alignment) {
    if (alignment == ALIGN_MIDDLE) {
        return "Middle";
    } else if (alignment == ALIGN_TOP) {
        return "Top";
    } else {   // if (alignment == ALIGN_BOTTOM)
        return "Bottom";
    }
}

HorizontalAlignment toHorizontalAlignment(const std::string &alignmentStr) {
    std::string alignLC = toLowerCase(trim(alignmentStr));
    if (stringContains(alignLC, "left") || stringContains(alignLC, "west")) {
        return ALIGN_LEFT;
    } else if (stringContains(alignLC, "right") || stringContains(alignLC, "east")) {
        return ALIGN_RIGHT;
    } else if (stringContains(alignLC, "stretch") || stringContains(alignLC, "justify")) {
        return ALIGN_HORIZONTAL_STRETCH;
    } else {
        return ALIGN_CENTER;
    }
}

Qt::Alignment toQtAlignment(HorizontalAlignment alignment) {
    return alignment == ALIGN_LEFT ? Qt::AlignLeft
                                   : alignment == ALIGN_CENTER ? Qt::AlignHCenter
                                                               : alignment == ALIGN_HORIZONTAL_STRETCH
                                                                 ? Qt::AlignJustify
                                                                 : Qt::AlignRight;
}

Qt::Alignment toQtAlignment(VerticalAlignment alignment) {
    return alignment == ALIGN_TOP ? Qt::AlignTop
                                  : alignment == ALIGN_MIDDLE ? Qt::AlignVCenter
                                                              : alignment == ALIGN_VERTICAL_STRETCH ? Qt::AlignTop
                                                                                                    : Qt::AlignBottom;
}

VerticalAlignment toVerticalAlignment(const std::string &alignmentStr) {
    std::string alignLC = toLowerCase(trim(alignmentStr));
    if (stringContains(alignLC, "top") || stringContains(alignLC, "north")) {
        return ALIGN_TOP;
    } else if (stringContains(alignLC, "bottom") || stringContains(alignLC, "south")) {
        return ALIGN_BOTTOM;
    } else if (stringContains(alignLC, "stretch") || stringContains(alignLC, "justify")) {
        return ALIGN_VERTICAL_STRETCH;
    } else {
        return ALIGN_MIDDLE;
    }
}

/*
 * Implementation notes: GPoint class
 * ----------------------------------
 * The GPoint class itself is entirely straightforward.  The relational
 * operators compare the x components first, followed by the y component.
 * The hashCode function computes the exclusive-or of the individual words.
 */

GPoint::GPoint(double x, double y)
        : _x(x),
          _y(y) {
    // empty
}

GPoint::GPoint(const Point &point) {
    _x = point.getX();
    _y = point.getY();
}

double GPoint::getX() const {
    return _x;
}

double GPoint::getY() const {
    return _y;
}

std::string GPoint::toString() const {
    std::ostringstream out;
    out << *this;
    return out.str();
}

std::ostream &operator<<(std::ostream &os, const GPoint &pt) {
    return os << "(" << pt._x << ", " << pt._y << ")";
}

bool operator==(const GPoint &p1, const GPoint &p2) {
    return floatingPointEqual(p1._x, p2._x)
           && floatingPointEqual(p1._y, p2._y);
}

bool operator!=(const GPoint &p1, const GPoint &p2) {
    return !(p1 == p2);
}

bool operator<(const GPoint &p1, const GPoint &p2) {
    return p1._x < p2._x || (floatingPointEqual(p1._x, p2._x) && p1._y < p2._y);
}

bool operator<=(const GPoint &p1, const GPoint &p2) {
    return p1 < p2 || p1 == p2;
}

bool operator>(const GPoint &p1, const GPoint &p2) {
    return p2 < p1;
}

bool operator>=(const GPoint &p1, const GPoint &p2) {
    return p1 > p2 || p1 == p2;
}

GPoint operator*(const GPoint &p, double scale) {
    return GPoint(p._x * scale, p._y * scale);
}

int hashCode(const GPoint &pt) {
    return hashCode(pt._x, pt._y);
}

/*
 * Implementation notes: GRectangle class
 * --------------------------------------
 * The GRectangle class itself is entirely straightforward.  The
 * relational operators compare the components in the following order:
 * x, y, width, height.  The hashCode function computes the exclusive-or
 * of the individual words.
 */

GRectangle::GRectangle(double x, double y, double width, double height)
        : _x(x),
          _y(y),
          _width(width),
          _height(height) {
    // empty
}

GRectangle::GRectangle(double x, double y, const GDimension &size)
        : _x(x),
          _y(y),
          _width(size.getWidth()),
          _height(size.getHeight()) {
    // empty
}

GRectangle::GRectangle(const GPoint &p, double width, double height)
        : _x(p.getX()),
          _y(p.getY()),
          _width(width),
          _height(height) {
    // empty
}

GRectangle::GRectangle(const GPoint &p, const GDimension &size)
        : _x(p.getX()),
          _y(p.getY()),
          _width(size.getWidth()),
          _height(size.getHeight()) {
    // empty
}

bool GRectangle::contains(double x, double y) const {
    return x >= this->_x && y >= this->_y
           && x < this->_x + _width
           && y < this->_y + _height;
}

bool GRectangle::contains(const GPoint &pt) const {
    return contains(pt.getX(), pt.getY());
}

bool GRectangle::contains(const GRectangle &rect) const {
    return contains(rect.getX(), rect.getY())
           && contains(rect.getX() + rect.getWidth() - 1, rect.getY() + rect.getHeight() - 1);
}

GRectangle GRectangle::enlargedBy(double amount) {
    return GRectangle(_x - amount, _y - amount, _width + 2 * amount, _height + 2 * amount);
}

double GRectangle::getX() const {
    return _x;
}

double GRectangle::getY() const {
    return _y;
}

double GRectangle::getWidth() const {
    return _width;
}

double GRectangle::getHeight() const {
    return _height;
}

bool GRectangle::intersects(const GRectangle &other) const {
    // check for non-intersecting x coordinates
    return !(getX() + getWidth() < other.getX()            // I am entirely left of him
             || getX() > other.getX() + other.getWidth()      // I am entirely right of him
             || getY() + getHeight() < other.getY()           // I am entirely above him
             || getY() > other.getY() + other.getHeight());   // I am entirely below him
}

bool GRectangle::isEmpty() const {
    return _width <= 0 || _height <= 0;
}

std::string GRectangle::toString() const {
    std::ostringstream out;
    out << *this;
    return out.str();
}

std::ostream &operator<<(std::ostream &os, const GRectangle &rect) {
    return os << "(" << rect.getX() << ", " << rect.getY() << ", "
              << rect.getWidth() << ", " << rect.getHeight() << ")";
}

bool operator==(const GRectangle &r1, const GRectangle &r2) {
    return floatingPointEqual(r1._x, r2._x)
           && floatingPointEqual(r1._y, r2._y)
           && floatingPointEqual(r1._width, r2._width)
           && floatingPointEqual(r1._height, r2._height);
}

bool operator!=(const GRectangle &r1, const GRectangle &r2) {
    return !(r1 == r2);
}

bool operator<(const GRectangle &r1, const GRectangle &r2) {
    return stanfordcpplib::collections::compareTo(
            r1._x, r2._x, r1._y, r2._y, r1._width, r2._width, r1._height, r2._height) < 0;
}

bool operator<=(const GRectangle &r1, const GRectangle &r2) {
    return r1 < r2 || r1 == r2;
}

bool operator>(const GRectangle &r1, const GRectangle &r2) {
    return r2 < r1;
}

bool operator>=(const GRectangle &r1, const GRectangle &r2) {
    return r1 > r2 || r1 == r2;
}

int hashCode(const GRectangle &r) {
    return hashCode(r._x, r._y, r._width, r._height);
}
