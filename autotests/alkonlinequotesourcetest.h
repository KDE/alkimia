/*
    SPDX-FileCopyrightText: 2018 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESOURCETEST_H
#define ALKONLINEQUOTESOURCETEST_H

#include <QtCore/QObject>

class AlkOnlineQuoteSource;

class AlkOnlineQuoteSourceTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void emptyCtor();
    void copyCtor();
    void assignOperator();
    void testReadWriteRemove();
    void testRename();
};

#endif // ALKONLINEQUOTESOURCETEST_H
