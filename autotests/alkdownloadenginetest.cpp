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
{
    m_url = QUrl(TEST_DOWNLOAD_URL);
    m_errorUrl = QUrl(TEST_DOWNLOAD_URL_ERROR);
}

void AlkDownloadEngineTest::testDownloadDefaultEngineError()
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    TestReceiver receiver(engine);

    QVERIFY(!engine->downloadUrl(m_errorUrl, AlkDownloadEngine::DefaultEngine));
    QVERIFY(receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void AlkDownloadEngineTest::testDownloadJavaScriptEngineError()
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    TestReceiver receiver(engine);

    QVERIFY(!engine->downloadUrl(m_errorUrl, AlkDownloadEngine::JavaScriptEngine));
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    QVERIFY(receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
#else
    QVERIFY(!receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(!receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
#endif
}

void AlkDownloadEngineTest::testDownloadDefaultEngineFinished()
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

    QVERIFY(engine->downloadUrl(m_url, AlkDownloadEngine::DefaultEngine));
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
    QVERIFY(receiver.gotDataMatches);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void AlkDownloadEngineTest::testDownloadJavaScriptEngineFinished()
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    QVERIFY(engine->downloadUrl(m_url, AlkDownloadEngine::JavaScriptEngine));
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
    QVERIFY(receiver.gotDataMatches);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
#else
    QVERIFY(!engine->downloadUrl(m_url, AlkDownloadEngine::JavaScriptEngine));
    QVERIFY(!receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(!receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
#endif
}

void AlkDownloadEngineTest::testDownloadDefaultEngineRedirected()
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

    QUrl url(m_url.toString() + "&redirect=1");
    QVERIFY(engine->downloadUrl(url, AlkDownloadEngine::DefaultEngine));
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
    QVERIFY(receiver.gotDataMatches);
    QVERIFY(receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
}

void AlkDownloadEngineTest::testDownloadJavaScriptEngineRedirected()
{
    QPointer<AlkDownloadEngine> engine = new AlkDownloadEngine;
    engine->setTimeout(20000);
    TestReceiver receiver(engine);

    QUrl url(m_url.toString() + "&redirect=1");
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    QVERIFY(engine->downloadUrl(url, AlkDownloadEngine::JavaScriptEngine));
    QVERIFY(!receiver.gotError);
    QVERIFY(receiver.gotFinished);
    QVERIFY(receiver.gotDataMatches);
    QVERIFY(receiver.gotRedirected);
    QVERIFY(receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
#else
    QVERIFY(!engine->downloadUrl(url, AlkDownloadEngine::JavaScriptEngine));
    QVERIFY(!receiver.gotError);
    QVERIFY(!receiver.gotFinished);
    QVERIFY(!receiver.gotDataMatches);
    QVERIFY(!receiver.gotRedirected);
    QVERIFY(!receiver.gotStarted);
    QVERIFY(!receiver.gotTimeout);
#endif
}

void AlkDownloadEngineTest::testDownloadTimout()
{
    QSKIP("not implemented yet - needs special reliable service", SkipAll);
}

// GUI is required by KIO and webxxx
QTEST_MAIN(AlkDownloadEngineTest)

#include "alkdownloadenginetest.moc"
