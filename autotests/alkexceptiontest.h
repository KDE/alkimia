/*
    SPDX-FileCopyrightText: 2002 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKEXCEPTIONTEST_H
#define ALKEXCEPTIONTEST_H

#include "alkexception.h"

#include <QtCore/QObject>

class AlkExceptionTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void cleanup();

    void testDefaultConstructor();

    void testConstructor();
};
#endif
