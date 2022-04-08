/*
    SPDX-FileCopyrightText: 2018 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesourcetest.h"

#include "alkonlinequotesource.h"
#include "test.h"

QTEST_GUILESS_MAIN(AlkOnlineQuoteSourceTest)

void AlkOnlineQuoteSourceTest::init()
{
}

void AlkOnlineQuoteSourceTest::cleanup()
{
}

void AlkOnlineQuoteSourceTest::emptyCtor()
{
    const QString emptyString;
    AlkOnlineQuoteSource *m = new AlkOnlineQuoteSource();
    QCOMPARE(m->name(), emptyString);
    QCOMPARE(m->isValid(), false);
    QCOMPARE(m->url(), emptyString);
    QCOMPARE(m->sym(), emptyString);
    QCOMPARE(m->price(), emptyString);
    QCOMPARE(m->date(), emptyString);
    QCOMPARE(m->dateformat(), emptyString);
    QCOMPARE(m->skipStripping(), false);
    QCOMPARE(m->isGHNS(), false);
    delete m;
}

void AlkOnlineQuoteSourceTest::copyCtor()
{
    AlkOnlineQuoteSource m0;
    m0.setName(QLatin1String("MyName"));
    m0.setUrl(QLatin1String("MyUrl"));

    AlkOnlineQuoteSource m1(m0);
    QCOMPARE(&m1 != &m0, true);
    QCOMPARE(m1.name(), QLatin1String("MyName"));
    QCOMPARE(m1.url(), QLatin1String("MyUrl"));

    m1.setName(QLatin1String("YourName"));
    QCOMPARE(m0.name(), QLatin1String("MyName"));
    QCOMPARE(m1.name(), QLatin1String("YourName"));
}

void AlkOnlineQuoteSourceTest::assignOperator()
{
    AlkOnlineQuoteSource m1;
    AlkOnlineQuoteSource m2;

    m1.setName(QLatin1String("MyName"));
    m2 = m1;
    m1.setName(QLatin1String("MyOtherName"));
    QCOMPARE(m1.name(), QLatin1String("MyOtherName"));
    QCOMPARE(m2.name(), QLatin1String("MyName"));
}

void AlkOnlineQuoteSourceTest::testReadWriteRemove()
{
    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia4);
    AlkOnlineQuoteSource m1(
        "test-currency",
        "https://fx-rate.net/%1/%2",
        QString(), // symbolregexp
        "1[ a-zA-Z]+=</span><br */?> *(\\d+\\.\\d+)",
        "updated\\s\\d+:\\d+:\\d+\\(\\w+\\)\\s+(\\d{1,2}/\\d{2}/\\d{4})",
        "%d/%m/%y",
        true // skip HTML stripping
    );
    m1.setProfile(&profile);
    m1.write();

    AlkOnlineQuoteSource m2("test-currency", &profile);
    QCOMPARE(m1.name(),          m2.name());
    QCOMPARE(m1.isValid(),       m2.isValid());
    QCOMPARE(m1.url(),           m2.url());
    QCOMPARE(m1.sym(),           m2.sym());
    QCOMPARE(m1.price(),         m2.price());
    QCOMPARE(m1.date(),          m2.date());
    QCOMPARE(m1.dateformat(),    m2.dateformat());
    QCOMPARE(m1.skipStripping(), m2.skipStripping());
    QCOMPARE(m1.isGHNS(),        m2.isGHNS());

    m2.remove();
    AlkOnlineQuoteSource m3("test-currency", &profile);
    QVERIFY(m3.url().isEmpty());
}

void AlkOnlineQuoteSourceTest::testRename()
{
    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia4);
    AlkOnlineQuoteSource m1(
        "test-currency",
        "https://fx-rate.net/%1/%2",
        QString(), // symbolregexp
        "1[ a-zA-Z]+=</span><br */?> *(\\d+\\.\\d+)",
        "updated\\s\\d+:\\d+:\\d+\\(\\w+\\)\\s+(\\d{1,2}/\\d{2}/\\d{4})",
        "%d/%m/%y",
        true // skip HTML stripping
    );
    m1.setProfile(&profile);
    m1.write();
    m1.rename("test-currency.new");

    // should be the same
    AlkOnlineQuoteSource m2("test-currency.new", &profile);
    QCOMPARE(m1.name(),          m2.name());
    QCOMPARE(m1.isValid(),       m2.isValid());
    QCOMPARE(m1.url(),           m2.url());
    QCOMPARE(m1.sym(),           m2.sym());
    QCOMPARE(m1.price(),         m2.price());
    QCOMPARE(m1.date(),          m2.date());
    QCOMPARE(m1.dateformat(),    m2.dateformat());
    QCOMPARE(m1.skipStripping(), m2.skipStripping());
    QCOMPARE(m1.isGHNS(),        m2.isGHNS());

    // should be empty
    AlkOnlineQuoteSource m3("test-currency", &profile);
    QVERIFY(m3.url().isEmpty());

    // cleanup
    m1.remove();
}
