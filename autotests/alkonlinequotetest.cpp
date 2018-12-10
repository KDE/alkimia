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
#include "alkonlinequotesprofile.h"
#include "alkquotereceiver.h"
#include "alkwebpage.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkOnlineQuoteTest)

void AlkOnlineQuoteTest::init()
{
    m_profile = new AlkOnlineQuotesProfile("alkimia",AlkOnlineQuotesProfile::Type::KMyMoney);
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

    QVERIFY(quote.launch("EUR USD", "EUR USD", m_profile->quoteSources().first()));
}
