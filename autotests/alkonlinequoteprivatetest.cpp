/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2020 Thomas Baumgart <tbaumgart@kde.org>

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequoteprivatetest.h"

#include "alkonlinequote_p.h"
#include "alkonlinequotesprofile.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkOnlineQuotePrivateTest)

AlkOnlineQuotePrivateTest::AlkOnlineQuotePrivateTest()
    : AlkOnlineQuote(new AlkOnlineQuotesProfile("alkimia"))
{
}

void AlkOnlineQuotePrivateTest::testParsePrice()
{
    AlkOnlineQuote::Private &p = d_ptr();

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12345.675"));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice(" 12 345.675"));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345.675"));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("109728364361224110982112411097810241109340924110963082411093707241109550624110955052411094404241109230324110949022411102301241110383123111056302311105629231110692823111118272311103026231110302523111005242311102323231110262223111003212311094920231109701923110927182311091017231109091623110909152311099"));
    QCOMPARE(p.m_price, 1.0972836436122411e+299);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("1.0971 &#8364; = &#36; Print Exchange Rate Chart Euro to Dollar - EUR/USD Invert DateExchange Rate12 Jan 241 EUR = 1.0982 USD11 Jan 241 EUR = 1.0978 USD10 Jan 241 EUR = 1.0934 USD09 Jan 241 EUR = 1.0963 USD08 Jan 241 EUR = 1.0937 USD07 Jan 241 EUR = 1.0955 USD06 Jan 241 EUR = 1.0955 USD05 Jan 241 EU"));
    QCOMPARE(p.m_price, 1.0971);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(!p.parsePrice("Print Exchange Rate Chart Euro to Dollar"));
    QCOMPARE(p.m_price, 0.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Price);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    // workaround for msvc 2048 character string limit
    QFile f(":/alkonlinequoteprivatetest.data");
    QVERIFY(f.open(QIODevice::ReadOnly));
    QVERIFY(!p.parsePrice(f.readAll()));
    QCOMPARE(p.m_price, 0.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Price);
}
