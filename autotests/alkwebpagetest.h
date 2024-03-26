/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKWEBPAGETEST_H
#define ALKWEBPAGETEST_H

#include <QtCore/QObject>

class AlkWebPageTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testToHtml();
    void testLoad();
    void testRedirected();
};

#endif // ALKWEBPAGETEST_H
