/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKDOMTEST_H
#define ALKDOMTEST_H

#include <QtCore/QObject>

class AlkDom;

class AlkDomTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void init();
    void cleanup();
    void testCreateDocument();
};

#endif
