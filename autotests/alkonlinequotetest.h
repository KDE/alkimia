/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTETEST_H
#define ALKONLINEQUOTETEST_H

#include <QtCore/QObject>

class AlkOnlineQuotesProfile;

class AlkOnlineQuoteTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void testQuoteSources();
    void testLaunch();
    void testLaunchWithBrowser();
private:
    AlkOnlineQuotesProfile *m_profile;
};

#endif // ALKONLINEQUOTETEST_H
