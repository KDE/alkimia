/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotetest.h"

#include "alkonlinequote.h"
#include "alkonlinequotesprofile.h"
#include "alkquotereceiver.h"
#include "alkwebpage.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkOnlineQuoteTest)

void AlkOnlineQuoteTest::init()
{
    m_profile = new AlkOnlineQuotesProfile("alkimia");
}

void AlkOnlineQuoteTest::cleanup()
{
    delete m_profile;
}

void AlkOnlineQuoteTest::testQuoteSources()
{
    QStringList sources = m_profile->quoteSources();
    qDebug() << sources;
    QVERIFY(sources.size() > 0);
}

void AlkOnlineQuoteTest::testLaunch()
{
    AlkOnlineQuote quote(m_profile);
    convertertest::AlkQuoteReceiver receiver(&quote);
    receiver.setVerbose(true);

    foreach(const QString &source, m_profile->quoteSources())
        if (!source.endsWith(QLatin1String(".webkit")))
            QVERIFY(quote.launch("EUR USD", "EUR USD", source));
}

void AlkOnlineQuoteTest::testLaunchWithBrowser()
{
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    AlkOnlineQuote quote(m_profile);
    convertertest::AlkQuoteReceiver receiver(&quote);
    receiver.setVerbose(true);

    foreach(const QString &source, m_profile->quoteSources())
        if (source.endsWith(QLatin1String(".webkit")))
            QVERIFY(quote.launch("EUR USD", "EUR USD", source));
#else
    QSKIP("Brower based tests skipped because requirements are not met", SkipAll);
#endif
}
