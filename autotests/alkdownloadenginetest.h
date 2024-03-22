/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKDOWNLOADENGINETEST_H
#define ALKDOWNLOADENGINETEST_H

#include <QUrl>
#include <QtCore/QObject>

class AlkDownloadEngineTest : public QObject
{
    Q_OBJECT
public:
    QUrl m_url;
    QUrl m_errorUrl;
    AlkDownloadEngineTest();

private Q_SLOTS:
    void testDownloadDefaultEngineFinished();
    void testDownloadJavaScriptEngineFinished();
    void testDownloadDefaultEngineError();
    void testDownloadJavaScriptEngineError();
    void testDownloadDefaultEngineRedirected();
    void testDownloadJavaScriptEngineRedirected();
    void testDownloadTimout();
};

#endif // ALKDOWNLOADENGINETEST_H
