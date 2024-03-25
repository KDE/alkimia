/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdownloadenginetest.h"

#include "alkdownloadengine.h"
#include "alktestdefs.h"
#include "alkwebpage.h"

#include "test.h"

class TestReceiver : public QObject
{
    Q_OBJECT
public:
    bool gotStarted{false};
    bool gotFinished{false};
    bool gotDataMatches{false};
    bool gotError{false};
    bool gotRedirected{false};
    bool gotTimeout{false};
    bool verbose{false};

    explicit TestReceiver(AlkDownloadEngine *engine)
    {
        QObject::connect(engine, SIGNAL(error(QUrl,QString)), this, SLOT(slotError(QUrl,QString)));
        QObject::connect(engine, SIGNAL(finished(QUrl,QString)), this, SLOT(slotFinished(QUrl,QString)));
        QObject::connect(engine, SIGNAL(finishedPage(QUrl,AlkWebPage*)), this, SLOT(slotFinishedPage(QUrl,AlkWebPage*)));
        QObject::connect(engine, SIGNAL(started(QUrl)), this, SLOT(slotStarted(QUrl)));
        QObject::connect(engine, SIGNAL(timeout(QUrl)), this, SLOT(slotTimeout(QUrl)));
        QObject::connect(engine, SIGNAL(redirected(QUrl,QUrl)), this, SLOT(slotRedirected(QUrl,QUrl)));
    }

public Q_SLOTS:
    void slotError(const QUrl &url, const QString &message)
    {
        gotError = true;
        if (verbose)
            qDebug() << Q_FUNC_INFO << url << message;
    }

    void slotFinished(const QUrl &url, const QString &data)
    {
        gotFinished = true;
        gotDataMatches = data.contains("</body></html>");
        if (verbose)
            qDebug() << Q_FUNC_INFO << url << data;
    }

    void slotFinishedPage(const QUrl &url, AlkWebPage *page)
    {
        QString match(QLatin1String("1 Bitcoin = "));
        QStringList entries = page->getAllElements("span");
        qDebug() << entries;
        gotFinished = entries.contains(match);
        entries = page->getAllElements("div h1 span");
        qDebug() << entries << entries.contains(match);
        entries = page->getAllElements("div#pair_today div div h1 span");
        qDebug() << entries << entries.contains(match);

        if (verbose)
            qDebug() << Q_FUNC_INFO << url << entries;
    }

    void slotStarted(const QUrl &url)
    {
        gotStarted = true;
        if (verbose)
            qDebug() << Q_FUNC_INFO << url;
    }

    void slotTimeout(const QUrl &url)
    {
        gotTimeout = true;
        if (verbose)
            qDebug() << Q_FUNC_INFO << url;
    }

    void slotRedirected(const QUrl &url, const QUrl &newurl)
    {
        gotRedirected = true;
        if (verbose)
            qDebug() << Q_FUNC_INFO << url << newurl;
    }
};

AlkDownloadEngineTest::AlkDownloadEngineTest()
    : m_url(TEST_DOWNLOAD_URL)
    , m_errorUrl(TEST_DOWNLOAD_URL_ERROR)
{
}

void testDownloadError(const QString &url, AlkDownloadEngine::Type type)
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(5000);
    TestReceiver receiver(engine);

#if defined(USE_KIO)
    QWARN("KIO does return false in case of errors");
    QVERIFY(engine->downloadUrl(url, type));
    QWARN("KIO does not emit the error signal");
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
#else
    QVERIFY(!engine->downloadUrl(url, type));
    QVERIFY(receiver.gotError);
    QVERIFY(!receiver.gotFinished);
#endif
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void testDownloadFailed(const QString &url, AlkDownloadEngine::Type type)
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

    QVERIFY(!engine->downloadUrl(url, type));
    QVERIFY(!receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(!receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void testDownloadFinished(const QString &url, AlkDownloadEngine::Type type)
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

    QVERIFY(engine->downloadUrl(url, type));
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
    QVERIFY(receiver.gotDataMatches);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void testDownloadRedirected(const QString &url, AlkDownloadEngine::Type type)
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

    QVERIFY(engine->downloadUrl(QUrl(url + "&redirect=1"), type));
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
    QVERIFY(receiver.gotDataMatches);
#if defined(USE_KIO) || defined(USE_WEBKIT)
    QWARN("This engine does not return a redirected url");
#else
    QVERIFY(receiver.gotRedirected);
#endif
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void testDownloadTimeout(const QString &url, AlkDownloadEngine::Type type)
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(5000);
    TestReceiver receiver(engine);

    QVERIFY(!engine->downloadUrl(QUrl(url + "&timeout=6"), type));
    QVERIFY(!receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotDataMatches);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(receiver.gotTimeout);
}

void AlkDownloadEngineTest::testDownloadEngineError()
{
#if defined(USE_QTNETWORK)
    testDownloadError(m_errorUrl, AlkDownloadEngine::QtEngine);
#elif defined(USE_KIO)
    testDownloadError(m_errorUrl, AlkDownloadEngine::KIOEngine);
#elif defined(USE_WEBKIT)
    testDownloadError(m_errorUrl, AlkDownloadEngine::WebKitEngine);
#elif defined(USE_WEBENGINE)
    testDownloadError(m_errorUrl, AlkDownloadEngine::WebEngine);
#else
    testDownloadError(m_errorUrl, AlkDownloadEngine::DefaultEngine);
#endif
}

void AlkDownloadEngineTest::testDownloadEngineFinished()
{
#if defined(USE_QTNETWORK)
    testDownloadFinished(m_url, AlkDownloadEngine::QtEngine);
#elif defined(USE_KIO)
    testDownloadFinished(m_url, AlkDownloadEngine::KIOEngine);
#elif defined(USE_WEBKIT)
    testDownloadFinished(m_url, AlkDownloadEngine::WebKitEngine);
#elif defined(USE_WEBENGINE)
    testDownloadFinished(m_url, AlkDownloadEngine::WebEngine);
#else
    testDownloadFinished(m_url, AlkDownloadEngine::DefaultEngine);
#endif
}

void AlkDownloadEngineTest::testDownloadEngineRedirected()
{
    #if defined(USE_QTNETWORK)
        testDownloadRedirected(m_url, AlkDownloadEngine::QtEngine);
    #elif defined(USE_KIO)
        testDownloadRedirected(m_url, AlkDownloadEngine::KIOEngine);
    #elif defined(USE_WEBKIT)
        testDownloadRedirected(m_url, AlkDownloadEngine::WebKitEngine);
    #elif defined(USE_WEBENGINE)
        testDownloadRedirected(m_url, AlkDownloadEngine::WebEngine);
    #else
        testDownloadRedirected(m_url, AlkDownloadEngine::DefaultEngine);
    #endif
}

void AlkDownloadEngineTest::testDownloadEngineTimeout()
{
    #if defined(USE_QTNETWORK)
        testDownloadTimeout(m_url, AlkDownloadEngine::QtEngine);
    #elif defined(USE_KIO)
        testDownloadTimeout(m_url, AlkDownloadEngine::KIOEngine);
    #elif defined(USE_WEBKIT)
        testDownloadTimeout(m_url, AlkDownloadEngine::WebKitEngine);
    #elif defined(USE_WEBENGINE)
        testDownloadTimeout(m_url, AlkDownloadEngine::WebEngine);
    #else
        testDownloadTimeout(m_url, AlkDownloadEngine::DefaultEngine);
    #endif
}

// GUI is required by KIO and webxxx
QTEST_MAIN(AlkDownloadEngineTest)

#include "alkdownloadenginetest.moc"
