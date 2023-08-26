/*
    SPDX-FileCopyrightText: 2011 Alvaro Soliverez asoliverez @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKCOMPANYTEST_H
#define ALKCOMPANYTEST_H

#include <QtCore/QObject>

class AlkCompany;

class AlkCompanyTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void init();
    void cleanup();
    void emptyCtor();
    void settersAndGetters();
    void copyCtor();
    void qDbusArgument();
};

#endif
