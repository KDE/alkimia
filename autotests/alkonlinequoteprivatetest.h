/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTEPRIVATETEST_H
#define ALKONLINEQUOTEPRIVATETEST_H


#include "alkonlinequote.h"

#include <QtCore/QObject>

class AlkOnlineQuotePrivateTest : public AlkOnlineQuote
{
    Q_OBJECT
public:
    AlkOnlineQuotePrivateTest();

private Q_SLOTS:
    void testParsePrice();
    void testParseQuoteCSVSingleLine();
    void testParseQuoteCSVMultipleLines();
    void testParseQuoteCSVDateRange();
    void testDateRangeInUrls();
    void testParseQuoteJson();
};

#endif // ALKONLINEQUOTEPRIVATETEST_H
