/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKDATEFORMATTEST_H
#define ALKDATEFORMATTEST_H

#include <QObject>

class AlkDateFormatTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testDateFormatKMyMoney();
    void testDateFormatSkrooge();
    void testValidFormatDetection();
};

#endif // ALKDATEFORMATTEST_H
