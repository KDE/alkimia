/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2020 Thomas Baumgart <tbaumgart@kde.org>

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

void checkSet(const AlkOnlineQuote::Errors& errors, QList<AlkOnlineQuote::Errors::Type> setFlags, QList<AlkOnlineQuote::Errors::Type> resetFlags)
{
    for (const auto& flag : setFlags) {
        QCOMPARE(errors & flag, true);
    }
    for (const auto& flag : resetFlags) {
        QCOMPARE(errors & flag, false);
    }
}

void AlkOnlineQuoteTest::testAlkOnlineQuoteErrorCtor()
{
    AlkOnlineQuote::Errors errors;

    QList<AlkOnlineQuote::Errors::Type> reset({AlkOnlineQuote::Errors::Data,
                                              AlkOnlineQuote::Errors::Date,
                                              AlkOnlineQuote::Errors::DateFormat,
                                              AlkOnlineQuote::Errors::Price,
                                              AlkOnlineQuote::Errors::Script,
                                              AlkOnlineQuote::Errors::Source,
                                              AlkOnlineQuote::Errors::Symbol,
                                              AlkOnlineQuote::Errors::Success,
                                              AlkOnlineQuote::Errors::URL});

    checkSet(errors, QList<AlkOnlineQuote::Errors::Type>(), reset);
}

void AlkOnlineQuoteTest::testAlkOnlineQuoteErrorCtorWithArg()
{
    AlkOnlineQuote::Errors errors(AlkOnlineQuote::Errors::Price);

    QList<AlkOnlineQuote::Errors::Type> reset({AlkOnlineQuote::Errors::Data,
        AlkOnlineQuote::Errors::Date,
        AlkOnlineQuote::Errors::DateFormat,
        AlkOnlineQuote::Errors::Script,
        AlkOnlineQuote::Errors::Source,
        AlkOnlineQuote::Errors::Symbol,
        AlkOnlineQuote::Errors::Success,
        AlkOnlineQuote::Errors::URL});

    QList<AlkOnlineQuote::Errors::Type> set({AlkOnlineQuote::Errors::Price});

    checkSet(errors, set, reset);

    errors = AlkOnlineQuote::Errors::None;

    reset.append(set);
    set.clear();

    checkSet(errors, set, reset);
}

void AlkOnlineQuoteTest::testAlkOnlineQuoteErrorSetter()
{
    AlkOnlineQuote::Errors errors;

    QList<AlkOnlineQuote::Errors::Type> reset({AlkOnlineQuote::Errors::Data,
                                              AlkOnlineQuote::Errors::Date,
                                              AlkOnlineQuote::Errors::DateFormat,
                                              AlkOnlineQuote::Errors::Price,
                                              AlkOnlineQuote::Errors::Script,
                                              AlkOnlineQuote::Errors::Source,
                                              AlkOnlineQuote::Errors::Symbol,
                                              AlkOnlineQuote::Errors::Success,
                                              AlkOnlineQuote::Errors::URL});

    QList<AlkOnlineQuote::Errors::Type> set;

    QList<AlkOnlineQuote::Errors::Type>::iterator it;
    for (it = reset.begin(); it != reset.end(); ) {
        const auto flag = *it;
        it = reset.erase(it);
        set.append(flag);
        errors |= flag;
        checkSet(errors, set, reset);
    }
}

void AlkOnlineQuoteTest::testAlkOnlineQuoteErrorCopy()
{
    AlkOnlineQuote::Errors errors;

    QList<AlkOnlineQuote::Errors::Type> reset({AlkOnlineQuote::Errors::None,
                                              AlkOnlineQuote::Errors::Date,
                                              AlkOnlineQuote::Errors::Price,
                                              AlkOnlineQuote::Errors::Source,
                                              AlkOnlineQuote::Errors::Success});

    QList<AlkOnlineQuote::Errors::Type> set({AlkOnlineQuote::Errors::Data,
                                            AlkOnlineQuote::Errors::DateFormat,
                                            AlkOnlineQuote::Errors::Script,
                                            AlkOnlineQuote::Errors::Symbol,
                                            AlkOnlineQuote::Errors::URL});

    for (const auto& flag : set) {
        errors |= flag;
    }
    checkSet(errors, set, reset);

    const auto errorsCopy(errors);
    checkSet(errorsCopy, set, reset);
}

void AlkOnlineQuoteTest::init()
{
    m_profile = new AlkOnlineQuotesProfile("Test");
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

    for (const QString &source : m_profile->quoteSources())
        if (!source.endsWith(QLatin1String(".webkit")))
            QVERIFY(quote.launch("EUR USD", "EUR USD", source));
}

void AlkOnlineQuoteTest::testLaunchWithBrowser()
{
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    AlkOnlineQuote quote(m_profile);
    convertertest::AlkQuoteReceiver receiver(&quote);
    receiver.setVerbose(true);

    for (const QString &source : m_profile->quoteSources())
        if (source.endsWith(QLatin1String(".webkit")))
            QVERIFY(quote.launch("EUR USD", "EUR USD", source));
#else
    QSKIP("Browser based tests skipped because requirements are not met", SkipAll);
#endif
}
