/*
    SPDX-FileCopyrightText: 2018 Thomas Baumgart <tbaumgart@kde.org>
    SPDX-License-Identifier: LGPL-2.1-or-later

    This file is part of libalkimia.
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
    QCOMPARE(m->idRegex(), emptyString);
    QCOMPARE(m->idSelector(), AlkOnlineQuoteSource::Symbol);
    QCOMPARE(m->priceDecimalSeparator(), AlkOnlineQuoteSource::Legacy);
    QCOMPARE(m->priceRegex(), emptyString);
    QCOMPARE(m->dataFormat(), AlkOnlineQuoteSource::HTML);
    QCOMPARE(m->dateRegex(), emptyString);
    QCOMPARE(m->dateFormat(), emptyString);
    QCOMPARE(m->isGHNS(), false);
    QCOMPARE(m->isReference(), false);
    delete m;
}

void AlkOnlineQuoteSourceTest::copyCtor()
{
    AlkOnlineQuoteSource m0;
    m0.setName(QLatin1String("MyName"));
    m0.setUrl(QLatin1String("MyUrl"));
    m0.setIdSelector(AlkOnlineQuoteSource::IdentificationNumber);

    AlkOnlineQuoteSource m1(m0);
    QCOMPARE(&m1 != &m0, true);
    QCOMPARE(m1.name(), QLatin1String("MyName"));
    QCOMPARE(m1.url(), QLatin1String("MyUrl"));
    QCOMPARE(m1.idSelector(), AlkOnlineQuoteSource::IdentificationNumber);

    m1.setName(QLatin1String("YourName"));
    QCOMPARE(m0.name(), QLatin1String("MyName"));
    QCOMPARE(m1.name(), QLatin1String("YourName"));
}

void AlkOnlineQuoteSourceTest::assignOperator()
{
    AlkOnlineQuoteSource m1;
    AlkOnlineQuoteSource m2;

    m1.setName(QLatin1String("MyName"));
    m1.setUrl(QLatin1String("MyUrl"));
    m1.setIdSelector(AlkOnlineQuoteSource::IdentificationNumber);

    m2 = m1;
    m1.setName(QLatin1String("MyOtherName"));
    QCOMPARE(m1.name(), QLatin1String("MyOtherName"));
    QCOMPARE(m2.name(), QLatin1String("MyName"));
    QCOMPARE(m2.url(), QLatin1String("MyUrl"));
    QCOMPARE(m2.idSelector(), AlkOnlineQuoteSource::IdentificationNumber);
}

void AlkOnlineQuoteSourceTest::testReadWriteRemove()
{
    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia4);
    AlkOnlineQuoteSource m1 = AlkOnlineQuoteSource::defaultCurrencyQuoteSource("test-currency");
    m1.setProfile(&profile);
    m1.write();

    AlkOnlineQuoteSource m2("test-currency", &profile);
    QCOMPARE(m1.name(), m2.name());
    QCOMPARE(m1.isValid(), m2.isValid());
    QCOMPARE(m1.url(), m2.url());
    QCOMPARE(m1.idRegex(), m2.idRegex());
    QCOMPARE(m1.idSelector(), m2.idSelector());
    QCOMPARE(m1.priceDecimalSeparator(), m2.priceDecimalSeparator());
    QCOMPARE(m1.priceRegex(), m2.priceRegex());
    QCOMPARE(m1.dataFormat(), m2.dataFormat());
    QCOMPARE(m1.dateRegex(), m2.dateRegex());
    QCOMPARE(m1.dateFormat(), m2.dateFormat());
    QCOMPARE(m1.isGHNS(), m2.isGHNS());

    m2.remove();
    AlkOnlineQuoteSource m3("test-currency", &profile);
    QVERIFY(m3.url().isEmpty());
}

void AlkOnlineQuoteSourceTest::testRename()
{
    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia4);
    AlkOnlineQuoteSource m1 = AlkOnlineQuoteSource::defaultCurrencyQuoteSource("test-currency");
    m1.setProfile(&profile);
    m1.write();
    m1.rename("test-currency.new");

    // should be the same
    AlkOnlineQuoteSource m2("test-currency.new", &profile);
    QCOMPARE(m1.name(), m2.name());
    QCOMPARE(m1.isValid(), m2.isValid());
    QCOMPARE(m1.url(), m2.url());
    QCOMPARE(m1.idRegex(), m2.idRegex());
    QCOMPARE(m1.idSelector(), m2.idSelector());
    QCOMPARE(m1.priceDecimalSeparator(), m2.priceDecimalSeparator());
    QCOMPARE(m1.priceRegex(), m2.priceRegex());
    QCOMPARE(m1.dataFormat(), m2.dataFormat());
    QCOMPARE(m1.dateRegex(), m2.dateRegex());
    QCOMPARE(m1.dateFormat(), m2.dateFormat());
    QCOMPARE(m1.isGHNS(), m2.isGHNS());

    // should be empty
    AlkOnlineQuoteSource m3("test-currency", &profile);
    QVERIFY(m3.url().isEmpty());

    // cleanup
    m1.remove();
}

// TODO: this does not work as a real GHNS source is required
void AlkOnlineQuoteSourceTest::testReference()
{
    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia5);
    AlkOnlineQuoteSource m1 = AlkOnlineQuoteSource::defaultCurrencyQuoteSource("test-currency");
    m1.setProfile(&profile);
    m1.setGHNS(true);
    m1.write();
    QVERIFY(!m1.isReference());

    AlkOnlineQuoteSource m2("test-currency.new", &profile);
    m2.setGHNS(false);
    m2.setReferenceName(m1.name());
    m2.write();
    QVERIFY(m1.name() != m2.name());
    QCOMPARE(m2.isReference(), false);

    // cleanup
    m1.remove();
    m2.remove();
}
