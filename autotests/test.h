#ifndef TEST_H
#define TEST_H

#include <QtTest/QtTest>

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
#define QTEST_GUILESS_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    QCoreApplication app(argc, argv); \
    TestObject tc; \
    return QTest::qExec(&tc, argc, argv); \
}
#endif

#endif // TEST_H
