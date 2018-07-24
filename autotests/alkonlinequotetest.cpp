/***************************************************************************
 *   Copyright 2018  Ralf Habacker ralf.habacker@freenet.de                *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#include "alkonlinequotetest.h"

#include "alkonlinequote.h"
#include "alkquotereceiver.h"

#include <QtTest/QtTest>
#include <QWebView>

QTEST_MAIN(AlkOnlineQuoteTest)

void AlkOnlineQuoteTest::init()
{
}

void AlkOnlineQuoteTest::cleanup()
{
}

void AlkOnlineQuoteTest::testQuoteSources()
{
    AlkOnlineQuoteSource::setProfile(new AlkOnlineQuotesProfile("alkimia",
                                                                AlkOnlineQuotesProfile::Type::
                                                                KMyMoney));

    QStringList sources = AlkOnlineQuote::quoteSources();
    qDebug() << sources;
    QVERIFY(sources.size() > 0);
}

void AlkOnlineQuoteTest::testLaunch()
{
    AlkOnlineQuote quote;
    convertertest::AlkQuoteReceiver receiver(&quote);
    QWebView *view = new QWebView;
    quote.setWebView(view);
    receiver.setVerbose(true);

    QVERIFY(quote.launch("EUR USD", "EUR USD", AlkOnlineQuote::quoteSources().first()));
    delete view;
}
