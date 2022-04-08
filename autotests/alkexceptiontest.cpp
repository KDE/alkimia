/*
    SPDX-FileCopyrightText: 2002 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkexceptiontest.h"

#include "test.h"

QTEST_GUILESS_MAIN(AlkExceptionTest)

void AlkExceptionTest::init()
{
}

void AlkExceptionTest::cleanup()
{
}

void AlkExceptionTest::testDefaultConstructor()
{
    const AlkException &e = ALKEXCEPTION("Message");
    QVERIFY(e.what() == "Message");
    QVERIFY(e.line() == __LINE__ - 2);
    QVERIFY(e.file() == __FILE__);
}

void AlkExceptionTest::testConstructor()
{
    AlkException e("New message", "Joe's file", 1234);
    QVERIFY(e.what() == "New message");
    QVERIFY(e.line() == 1234);
    QVERIFY(e.file() == "Joe's file");
}
