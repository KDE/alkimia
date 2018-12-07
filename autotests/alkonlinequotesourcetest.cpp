/***************************************************************************
 *   Copyright 2018  Thomas Baumgart  tbaumgart@kde.org                    *
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

#include "alkonlinequotesourcetest.h"

#include "alkonlinequotesource.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkOnlineQuoteSourceTest)

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
