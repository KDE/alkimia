#ifndef TEST_H
#define TEST_H

#include <QtTest/QtTest>

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
#define QTRY_LOOP_IMPL(expr, timeoutValue, step) \
    if (!(expr)) { \
        QTest::qWait(0); \
    } \
    int qt_test_i = 0; \
    for (; qt_test_i < timeoutValue && !(expr); qt_test_i += step) { \
        QTest::qWait(step); \
    }

#define QTRY_TIMEOUT_DEBUG_IMPL(expr, timeoutValue, step)\
    if (!(expr)) { \
        QTRY_LOOP_IMPL((expr), (2 * timeoutValue), step);\
        if (expr) { \
            QString msg = QString::fromUtf8("QTestLib: This test case check (\"%1\") failed because the requested timeout (%2 ms) was too short, %3 ms would have been sufficient this time."); \
            msg = msg.arg(QString::fromUtf8(#expr)).arg(timeoutValue).arg(timeoutValue + qt_test_i); \
            QFAIL(qPrintable(msg)); \
        } \
    }

// Ideally we'd use qWaitFor instead of QTRY_LOOP_IMPL, but due
// to a compiler bug on MSVC < 2017 we can't (see QTBUG-59096)
#define QTRY_IMPL(expr, timeout)\
    const int qt_test_step = timeout < 350 ? timeout / 7 + 1 : 50; \
    const int qt_test_timeoutValue = timeout; \
    { QTRY_LOOP_IMPL((expr), qt_test_timeoutValue, qt_test_step); } \
    QTRY_TIMEOUT_DEBUG_IMPL((expr), qt_test_timeoutValue, qt_test_step)\

// Will try to wait for the expression to become true while allowing event processing
#define QTRY_VERIFY_WITH_TIMEOUT(expr, timeout) \
do { \
    QTRY_IMPL((expr), timeout);\
    QVERIFY(expr); \
} while (false)

#define QTRY_VERIFY(expr) QTRY_VERIFY_WITH_TIMEOUT((expr), 5000)

// Will try to wait for the expression to become true while allowing event processing
#define QTRY_VERIFY2_WITH_TIMEOUT(expr, messageExpression, timeout) \
do { \
    QTRY_IMPL((expr), timeout);\
    QVERIFY2(expr, messageExpression); \
} while (false)

#define QTRY_VERIFY2(expr, messageExpression) QTRY_VERIFY2_WITH_TIMEOUT((expr), (messageExpression), 5000)

// Will try to wait for the comparison to become successful while allowing event processing
#define QTRY_COMPARE_WITH_TIMEOUT(expr, expected, timeout) \
do { \
    QTRY_IMPL(((expr) == (expected)), timeout);\
    QCOMPARE((expr), expected); \
} while (false)

#define QTEST_GUILESS_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    QCoreApplication app(argc, argv); \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}
#endif

#endif // TEST_H
