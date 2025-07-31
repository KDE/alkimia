/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2018-2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkdateformattest.h"

#include "alkdateformat.h"
#include "test.h"

QTEST_GUILESS_MAIN(AlkDateFormatTest)

void AlkDateFormatTest::testDateFormatKMyMoney()
{
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

    format = AlkDateFormat("%u");
    QCOMPARE(format.convertString("1714827042", false), QDate(2024, 5, 4));
    QCOMPARE(format.convertString("ABC", false), QDate());
    QCOMPARE(format.convertString("-1234", false), QDate(1970, 1, 1));

    format = AlkDateFormat("%ud");
    QCOMPARE(format.convertString("19847", false), QDate(2024, 5, 4));
    QCOMPARE(format.convertString("ABC", false), QDate());
    QCOMPARE(format.convertString("-1234", false), QDate(1966, 8, 16));

    format = AlkDateFormat("%um");
    QCOMPARE(format.convertString("1753875094117", false), QDate(2025, 7, 30));
    QCOMPARE(format.convertString("ABC", false), QDate());
    QCOMPARE(format.convertString("-1234", false), QDate(1970, 1, 1));
}

void AlkDateFormatTest::testDateFormatSkrooge()
{
    const QLocale defaultLocale = QLocale();
    QLocale::setDefault(QLocale("de_DE"));

    AlkDateFormat format1("M-d-yyyy");
    AlkDateFormat format2("MMM-dd-yyyy");
    AlkDateFormat format3("MMMM-dd-yyyy");

    QCOMPARE(format1.convertString("1-5-2005"), QDate(2005, 1, 5));

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QCOMPARE(format2.convertString("Dez.-15-2005"), QDate(2005, 12, 15));
    QCOMPARE(format2.convertString("Dez-15-2005"), QDate(2005, 12, 15));
#endif
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
    QCOMPARE(format1.convertString("jan 15, 2005 21:02 GMT"), QDate(2005, 1, 15));
    QCOMPARE(format2.convertString("august 25, 2005"), QDate(2005, 8, 25));
    QCOMPARE(format2.convertString("august 25, 2005 21:02 GMT"), QDate(2005, 8, 25));

    // FinancialTimesFund
    format1 = AlkDateFormat("MMM dd yyyy");
    QCOMPARE(format1.convertString("jan 15 2005 21:02 GMT"), QDate(2005, 1, 15));
    QCOMPARE(format1.convertString("dec 15 2005 21:02 GMT"), QDate(2005, 12, 15));

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

    QLocale::setDefault(defaultLocale);
}

void AlkDateFormatTest::testValidFormatDetection()
{
    AlkDateFormat format1("M-d-yyyy");
    QCOMPARE(format1.lastError(), AlkDateFormat::NoError);
    QCOMPARE(format1.lastErrorMessage(), QString());

    format1 = AlkDateFormat("bla");
    QCOMPARE(format1.lastError(), AlkDateFormat::NoError);
    QCOMPARE(format1.lastErrorMessage(), QString());

    QDate date;
    date = format1.convertString("1105747200");
    QCOMPARE(date.isValid(), false);
    QCOMPARE(format1.lastError(), AlkDateFormat::InvalidFormatString);
}
