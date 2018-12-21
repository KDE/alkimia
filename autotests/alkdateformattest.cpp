/***************************************************************************
 *   Copyright 2004  Ace Jones <acejones@users.sourceforge.net>            *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#include "alkdateformattest.h"
#include "alkdateformat.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkDateFormatTest)

void AlkDateFormatTest::testDateFormatKMyMoney()
{
  try {
    AlkDateFormat format("%mm-%dd-%yyyy");

    QCOMPARE(format.convertString("1-5-2005"), QDate(2005, 1, 5));
    QCOMPARE(format.convertString("jan-15-2005"), QDate(2005, 1, 15));
    QCOMPARE(format.convertString("august-25-2005"), QDate(2005, 8, 25));

    format = AlkDateFormat("%mm/%dd/%yy");

    QCOMPARE(format.convertString("1/5/05"), QDate(2005, 1, 5));
    QCOMPARE(format.convertString("jan/15/05"), QDate(2005, 1, 15));
    QCOMPARE(format.convertString("august/25/05"), QDate(2005, 8, 25));

    format = AlkDateFormat("%d\\.%m\\.%yy");

    QCOMPARE(format.convertString("1.5.05"), QDate(2005, 5, 1));
    QCOMPARE(format.convertString("15.jan.05"), QDate(2005, 1, 15));
    QCOMPARE(format.convertString("25.august.05"), QDate(2005, 8, 25));

    format = AlkDateFormat("%yyyy\\\\%dddd\\\\%mmmmmmmmmmm");

    QCOMPARE(format.convertString("2005\\31\\12"), QDate(2005, 12, 31));
    QCOMPARE(format.convertString("2005\\15\\jan"), QDate(2005, 1, 15));
    QCOMPARE(format.convertString("2005\\25\\august"), QDate(2005, 8, 25));

    format = AlkDateFormat("%m %dd, %yyyy");

    QCOMPARE(format.convertString("jan 15, 2005"), QDate(2005, 1, 15));
    QCOMPARE(format.convertString("august 25, 2005"), QDate(2005, 8, 25));
    QCOMPARE(format.convertString("january 1st, 2005"), QDate(2005, 1, 1));

    format = AlkDateFormat("%m %d %y");

    QCOMPARE(format.convertString("12/31/50", false, 2000), QDate(1950, 12, 31));
    QCOMPARE(format.convertString("1/1/90", false, 2000), QDate(1990, 1, 1));
    QCOMPARE(format.convertString("december 31st, 5", false), QDate(2005, 12, 31));
  } catch (const AlkException &e) {
    QFAIL(qPrintable(e.what()));
  }
}

void AlkDateFormatTest::testDateFormatSkrooge()
{
  const QLocale defaultLocale = QLocale();
  QLocale::setDefault(QLocale("de_DE"));
  try {
    AlkDateFormat format1("M-d-yyyy");
    AlkDateFormat format2("MMM-dd-yyyy");
    AlkDateFormat format3("MMMM-dd-yyyy");

    QCOMPARE(format1.convertString("1-5-2005"), QDate(2005, 1, 5));
    QCOMPARE(format2.convertString("Dez.-15-2005"), QDate(2005, 12, 15));
    QCOMPARE(format2.convertString("Dez-15-2005"), QDate(2005, 12, 15));
    QCOMPARE(format2.convertString("Dec-15-2005"), QDate(2005, 12, 15));
    QCOMPARE(format3.convertString("august-25-2005"), QDate(2005, 8, 25));

    format1 = AlkDateFormat("M/d/yy");
    format2 = AlkDateFormat("MMM/dd/yy");
    format3 = AlkDateFormat("MMMM/dd/yy");

    QCOMPARE(format1.convertString("1/5/05"), QDate(2005, 1, 5));
    QCOMPARE(format2.convertString("jan/15/05"), QDate(2005, 1, 15));
    QCOMPARE(format3.convertString("august/25/05"), QDate(2005, 8, 25));

    format1 = AlkDateFormat("d.M.yy");
    format2 = AlkDateFormat("dd.MMM.yy");
    format3 = AlkDateFormat("dd.MMMM.yy");

    QCOMPARE(format1.convertString("1.5.05"), QDate(2005, 5, 1));
    QCOMPARE(format2.convertString("15.jan.05"), QDate(2005, 1, 15));
    QCOMPARE(format3.convertString("25.august.05"), QDate(2005, 8, 25));

    format1 = AlkDateFormat("yyyy\\dd\\MM");
    format2 = AlkDateFormat("yyyy\\dd\\MMM");
    format3 = AlkDateFormat("yyyy\\dd\\MMMM");

    QCOMPARE(format1.convertString("2005\\31\\12"), QDate(2005, 12, 31));
    QCOMPARE(format2.convertString("2005\\15\\jan"), QDate(2005, 1, 15));
    QCOMPARE(format3.convertString("2005\\25\\august"), QDate(2005, 8, 25));

    format1 = AlkDateFormat("MMM dd, yyyy");
    format2 = AlkDateFormat("MMMM dd, yyyy");
    format3 = AlkDateFormat("MMMM ddd, yyyy");

    QCOMPARE(format1.convertString("jan 15, 2005"), QDate(2005, 1, 15));
    QCOMPARE(format2.convertString("august 25, 2005"), QDate(2005, 8, 25));

    format1 = AlkDateFormat("UNIX");
    QCOMPARE(format1.convertString("1105747200"), QDate(2005, 1, 15));

    // not supported
#if 0
    QCOMPARE(format3.convertString("january 1st, 2005"), QDate(2005, 1, 1));
    format = AlkDateFormat("%m %d %y");

    QCOMPARE(format.convertString("12/31/50", false, 2000), QDate(1950, 12, 31));
    QCOMPARE(format.convertString("1/1/90", false, 2000), QDate(1990, 1, 1));
    QCOMPARE(format.convertString("december 31st, 5", false), QDate(2005, 12, 31));
#endif
} catch (const AlkException &e) {
    QFAIL(qPrintable(e.what()));
  }
  QLocale::setDefault(defaultLocale);
}
