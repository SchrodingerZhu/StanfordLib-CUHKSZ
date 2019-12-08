/*
 * File: gfont.cpp
 * ---------------
 *
 * @author Marty Stepp
 * @version 2019/04/30
 * - added changeFontSize for a GText*
 * @version 2018/09/23
 * - added macro checks to improve compatibility with old Qt versions
 * @version 2018/09/14
 * - added boldFont, italicFont
 * @version 2018/08/23
 * - renamed to gfont.cpp to replace Java version
 * @version 2018/07/05
 * - initial version
 */

#include <graphics/gfont.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <QtGlobal>
#include <system/error.h>
#include <util/require.h>
#include <util/strlib.h>
#include <collections/vector.h>

GFont::GFont() {
    // empty
}

void GFont::boldFont(GInteractor* interactor) {
    require::nonNull(interactor, "GFont::boldFont", "interactor");
    QFont newFont = deriveQFont(toQFont(interactor->getFont()), /* weight */ QFont::Bold);
    interactor->setFont(newFont);
}

void GFont::changeFontSize(GInteractor* interactor, int dsize) {
    require::nonNull(interactor, "GFont::changeFontSize", "interactor");
    QFont newFont = changeFontSize(toQFont(interactor->getFont()), dsize);
    interactor->setFont(newFont);
}

void GFont::changeFontSize(GText* label, int dsize) {
    require::nonNull(label, "GFont::changeFontSize", "label");
    QFont newFont = changeFontSize(toQFont(label->getFont()), dsize);
    label->setFont(newFont);
}

QFont GFont::changeFontSize(const QFont& font, int dsize) {
    int newSize = std::max(1, font.pointSize() + dsize);
    return QFont(font.family(), newSize, font.weight(), font.italic());
}

QFont GFont::deriveQFont(const QFont& font, QFont::Weight weight, int size) {
    return QFont(font.family(), size, weight, /* italic */ font.italic());
}

QFont GFont::deriveQFont(const QFont& font, const std::string& fontFamily, QFont::Weight weight, int size) {
    return QFont(QString::fromStdString(fontFamily), size, weight, /* italic */ font.italic());
}

QFont GFont::deriveQFont(const std::string& font, QFont::Weight weight, int size) {
    return deriveQFont(toQFont(font), weight, size);
}

QFont GFont::deriveQFont(const std::string& font, const std::string& fontFamily, QFont::Weight weight, int size) {
    return deriveQFont(toQFont(font), fontFamily, weight, size);
}

// http://doc.qt.io/qt-5/qfont.html#StyleHint-enum
QFont::StyleHint GFont::getStyleHint(const std::string& fontFamily) {
    std::string fontFamilyStr = toLowerCase(trim(fontFamily));
    if (fontFamilyStr == "arial"
            || fontFamilyStr == "helvetica"
            || fontFamilyStr == "sansserif"
            || fontFamilyStr == "sans-serif") {
        return QFont::Helvetica;
    } else if (fontFamilyStr == "serif"
            || fontFamilyStr == "times"
            || fontFamilyStr == "times roman"
            || fontFamilyStr == "times new roman") {
        return QFont::Times;
    } else if (fontFamilyStr == "courier"
            || fontFamilyStr == "courier new"
            || fontFamilyStr == "consolas"
            || fontFamilyStr == "monospace"
            || fontFamilyStr == "monospaced"
            || endsWith(fontFamilyStr, " mono")) {
        return QFont::Monospace;
    } else if (fontFamilyStr == "comic sans"
            || fontFamilyStr == "comic sans ms"
            || fontFamilyStr == "fantasy") {
        return QFont::Fantasy;
    } else if (fontFamilyStr == "cursive") {
        return QFont::Cursive;
    } else if (fontFamilyStr == "system") {
        return QFont::System;
    } else {
        return QFont::AnyStyle;
    }
}

void GFont::italicFont(GInteractor* interactor) {
    require::nonNull(interactor, "GFont::boldFont", "interactor");
    QFont oldFont = toQFont(interactor->getFont());
    QFont newFont(oldFont.family(), oldFont.pointSize(), oldFont.weight(), /* italic */ true);
    interactor->setFont(newFont);
}

// example font string:
// "Courier New-Bold-12"
std::string GFont::toFontString(const QFont& font) {
    std::ostringstream out;
    out << font.family().toStdString();

    // http://doc.qt.io/qt-5/qfont.html#Weight-enum
    bool dashPrinted = false;
    switch (font.weight()) {
        case QFont::DemiBold:
        case QFont::Bold:
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        case QFont::ExtraBold:
#endif // QT_VERSION
        case QFont::Black:
            out << "-bold";
            dashPrinted = true;
            break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        case QFont::Thin:
        case QFont::ExtraLight:
#endif // QT_VERSION
        case QFont::Light:
        case QFont::Normal:
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        case QFont::Medium:
#endif // QT_VERSION
        default:
            // not bold
            break;
    }
    if (font.italic()) {
        if (!dashPrinted) {
            // dashPrinted = true;
            out << "-";
        }
        out << "italic";
    }

    out << "-" << font.pointSize();
    return out.str();
}

// example font string:
// "Courier New-Bold-12"
QFont GFont::toQFont(const std::string& fontString) {
    Vector<std::string> tokens = stringSplit(trim(fontString), "-");
    if (tokens.isEmpty()) {
        return QFont();
    }

    std::string fontFamily = "";
    bool fontBold = false;
    bool fontItalic = false;
    int fontSize = 12;   // 12pt standard font size

    if (!tokens.isEmpty()) {
        fontFamily = trim(tokens.removeFront());
        if (fontFamily == "*") {
            fontFamily = "SansSerif";
        }
    }
    for (int i = 0; i < 2; i++) {
        if (!tokens.isEmpty()) {
            // tokens 2-3 can be size-style or style-size
            std::string fontWeightStr = toLowerCase(trim(tokens.removeFront()));
            if (stringIsInteger(fontWeightStr)) {
                fontSize = stringToInteger(fontWeightStr);
            }
            if (stringContains(fontWeightStr, "bold")) {
                fontBold = true;
            }
            if (stringContains(fontWeightStr, "italic")) {
                fontItalic = true;
            }
        }
    }

    QFont font = QFont(QString::fromStdString(fontFamily),
                       fontSize,
                       fontBold ? QFont::Bold : QFont::Normal,
                       fontItalic);
    font.setStyleHint(getStyleHint(fontFamily));
    return font;
}

QFont GFont::toQFont(const QFont& basisFont, const std::string& fontString) {
    Vector<std::string> tokens = stringSplit(trim(fontString), "-");
    if (tokens.isEmpty()) {
        return QFont();
    }

    std::string fontFamily = "";
    bool fontBold = basisFont.bold();
    bool fontItalic = basisFont.italic();
    int fontSize = basisFont.pointSize();   // 12pt standard font size

    if (!tokens.isEmpty()) {
        fontFamily = trim(tokens.removeFront());
        if (fontFamily == "*") {
            fontFamily = basisFont.family().toStdString();
        }
    }
    for (int i = 0; i < 2; i++) {
        if (!tokens.isEmpty()) {
            // tokens 2-3 can be size-style or style-size
            std::string fontWeightStr = toLowerCase(trim(tokens.removeFront()));
            if (stringIsInteger(fontWeightStr)) {
                fontSize = stringToInteger(fontWeightStr);
            } else if (stringContains(fontWeightStr, "bold")) {
                fontBold = true;
            } else if (stringContains(fontWeightStr, "italic")) {
                fontItalic = true;
            }
        }
    }

    QFont font = QFont(QString::fromStdString(fontFamily),
                       fontSize,
                       fontBold ? QFont::Bold : QFont::Normal,
                       fontItalic);
    font.setStyleHint(getStyleHint(fontFamily));
    return font;
}
