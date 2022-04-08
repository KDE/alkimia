/*
    SPDX-FileCopyrightText: 2011 Alvaro Soliverez asoliverez @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKQUOTEITEMTEST_H
#define ALKQUOTEITEMTEST_H

#include <QtCore/QObject>

class AlkQuoteItem;

class AlkQuoteItemTest : public QObject
{
    Q_OBJECT

private slots:

    void init();
    void cleanup();
    void emptyCtor();
    void settersAndGetters();
    void copyCtor();
    void qDbusArgument();
};

#endif
