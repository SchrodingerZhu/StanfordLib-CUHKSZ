//
// Created by schrodinger on 12/11/19.
//

#include <util/strlib.h>
#include <private/init.h>
#include "test_header.h"
#define USE_SYNC_POOL_RESOURCE
class TestStrlib: public QObject
{
Q_OBJECT
private slots:
    static void basicFunctions();
    static void identityChecking();
    static void stringMatching();
    static void stringOperations();
};


void TestStrlib::basicFunctions() {
    QCOMPARE(boolToString(true), "true");
    QCOMPARE(boolToString(1), "true");
    QCOMPARE(boolToString(false),  "false");
    QCOMPARE(boolToString(0),  "false");
    for(char i = '0'; i <= '9'; ++i) {
        QCOMPARE(charToInteger(i), i - '0');
    }
    for(char i = 32; i <= 126; ++i) {
        auto temp = charToString(i);
        Q_ASSERT(temp.length() == 1 && temp[0] == i);
    }
    QCOMPARE(doubleToString(123.5), "123.5");
    QCOMPARE(doubleToString(-114.514), "-114.514");
    QCOMPARE(doubleToString(qQNaN()), "NAN");
    QCOMPARE(doubleToString(qInf()), "INF");
    QVERIFY(startsWith("1234 5678", '1'));
    QVERIFY(startsWith("1234 5678", "1234"));
    QVERIFY(endsWith("1234 5678", "5678"));
    QVERIFY(endsWith("1234 5678", '8'));
}

void TestStrlib::identityChecking() {
    QVERIFY(stringIsBool("true"));
    QVERIFY(stringIsBool("false"));
    QVERIFY(!stringIsBool("fales"));
    QVERIFY(stringIsDouble("123.4"));
    QVERIFY(stringIsDouble("-123.4"));
    QVERIFY(!stringIsDouble("abs"));
    QVERIFY(!stringIsInteger("true"));
    QVERIFY(!stringIsInteger("114.514"));
    QVERIFY(!stringIsInteger("boy"));
    QVERIFY(stringIsInteger("123"));
    QVERIFY(stringIsLong("0xFF", 16));
    QVERIFY(!stringIsLong("0xKK", 16));
}

void TestStrlib::stringMatching() {
    QVERIFY(stringContains("1asdfwcekjf wei fcwinmofcwxe", 'f'));
    QVERIFY(!stringContains("@@@@%%%%%@#@#@##@#@#@##@#@#@", 'f'));
    QVERIFY(stringContains("@@@@%%%%%@#@ #@##@sadf#@#@##@#@#@ ", "sadf"));
    QVERIFY(!stringContains("@@@@%%%%%@#@ #@##@sadf#@#@##@#@#@ ", {'a', 'a', 'a'}));
    QCOMPARE(stringIndexOf("@@@@%%%%%@#@ #@##@sadf#@#@##@#@#@ ", '@'), 0);
    QCOMPARE(stringIndexOf("0123456", '6'), 6);
    QCOMPARE(stringIndexOf("0123456", '#'), std::string::npos);
    QCOMPARE(stringIndexOf("0123456", "1235"), std::string::npos);
    QCOMPARE(stringIndexOf("0123456", "345"), 3);
    QCOMPARE(stringLastIndexOf("0123456", '6'), 6);
    QCOMPARE(stringLastIndexOf("0123456", '#'), std::string::npos);
    QCOMPARE(stringLastIndexOf("0123456", "1235"), std::string::npos);
    QCOMPARE(stringLastIndexOf("0123456", "345"), 3);
    QCOMPARE(stringLastIndexOf("012345612", '1'), 7);
    QCOMPARE(stringLastIndexOf("012345612", "12"), 7);
}

void TestStrlib::stringOperations() {
    QCOMPARE(stringJoin({"1", "2", "3"}, ""), "123");
    QCOMPARE(stringJoin({"1", "2", "3"}, ','), "1,2,3");
    QCOMPARE(stringJoin({"1", "2", "3"}, ", "), "1, 2, 3");
    QCOMPARE(stringReplace("*, 123, *", "*", "123"), "123, 123, 123");
    QCOMPARE(stringReplace("*, 123, *", "#", "123", 2), "*, 123, *");
    QCOMPARE(stringReplace("*, 123, *", '*', '1', 1), "1, 123, *");
    QCOMPARE(stringReplace("*, 123, *", '#', '1'), "*, 123, *");
}

LIB_TEST_MAIN(TestStrlib)
#include "test_strlib.moc"
#undef USE_SYNC_POOL_RESOURCE
