﻿/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdownloadengine.h"

#include "alkimia/alkversion.h"
#include "alkwebpage.h"

#include <QEventLoop>
#include <QTimer>
#ifdef BUILD_WITH_QTNETWORK
#include <QNetworkReply>
#include <QNetworkAccessManager>
#define KJob void
#else
    #include <KIO/Scheduler>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <KIO/Job>
    #include <QTemporaryFile>
#else
    #include <kio/netaccess.h>
#endif
#include <QtDebug>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <klocalizedstring.h>
#else
    #include <KDebug>
    #include <KGlobal>
    #include <KLocale>
#endif

class AlkDownloadEngine::Private : public QObject {
    Q_OBJECT
public:
    enum Result {
        NoError = 0,
        Redirect = 2,
        TimeoutError,
        FileNotFoundError,
        OtherError,
    };

    AlkDownloadEngine *m_p;
    QString m_acceptLanguage;
    QEventLoop *m_eventLoop{nullptr};
    Type m_type{DefaultEngine};
    int m_timeout{-1};
    QUrl m_url;
    bool downloadUrl(const QUrl& url);
    bool downloadUrlWithJavaScriptEngine(const QUrl &url);
    AlkWebPage *m_webPage{nullptr};
    bool m_webPageCreated{false};

    Private(AlkDownloadEngine *p, QObject *parent)
        : QObject(parent)
        , m_p(p)
    {}

    ~Private()
    {
        if (m_webPageCreated)
            delete m_webPage;
    }

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static int dbgArea()
    {
        static int s_area = KDebug::registerArea("Alkimia (AlkOnlineQuote)");
        return s_area;
    }
#else
    static int dbgArea() {return 0; }
#endif

public Q_SLOTS:
#ifdef BUILD_WITH_QTNETWORK
    void downloadUrlDone(QNetworkReply *reply);
#else
    void downloadUrlDone(KJob *reply);
#endif
#if QT_VERSION < QT_VERSION_CHECK(5,0,0) || defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    void slotFinishedJavaScriptEngine(bool ok);
#endif
    void slotLoadStarted();
    void slotLoadTimeout();
    void slotUrlChanged(const QUrl &url);
};

void AlkDownloadEngine::Private::slotLoadStarted()
{
    Q_EMIT m_p->started(m_url);
}

void AlkDownloadEngine::Private::slotLoadTimeout()
{
    Q_EMIT m_p->timeout(m_url);
    if (m_eventLoop)
        m_eventLoop->exit(Result::TimeoutError);
}

void AlkDownloadEngine::Private::slotUrlChanged(const QUrl &url)
{
    Q_EMIT m_p->redirected(m_url, url);
    m_url = url;
}

#ifdef BUILD_WITH_QTNETWORK
void AlkDownloadEngine::Private::downloadUrlDone(QNetworkReply *reply)
{
    Result result = Result::NoError;
    if (reply->error() == QNetworkReply::NoError) {
        QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (!newUrl.isEmpty() && newUrl != reply->url()) {
            slotUrlChanged(reply->url().resolved(newUrl));
            result = Result::Redirect;
        } else {
            //kDebug(dbgArea()) << "Downloaded data from" << reply->url();
            Q_EMIT m_p->finished(reply->url(), reply->readAll());
        }
    } else {
        Q_EMIT m_p->error(reply->url(), reply->errorString());
        result = Result::OtherError;
    }
    m_eventLoop->exit(result);
}

bool AlkDownloadEngine::Private::downloadUrl(const QUrl &url)
{
    QNetworkAccessManager manager(this);
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadUrlDone(QNetworkReply*)));

    m_url = url;
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "alkimia " ALK_VERSION_STRING);
    if (!m_acceptLanguage.isEmpty())
        request.setRawHeader("Accept-Language: ", m_acceptLanguage.toLocal8Bit());
    manager.get(request);

    m_eventLoop = new QEventLoop;
    Q_EMIT m_p->started(m_url);

    if (m_timeout != -1)
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));

    Result result = static_cast<Result>(m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents));
    delete m_eventLoop;
    m_eventLoop = nullptr;
    if (result == Result::Redirect) {
        QNetworkRequest req;
        req.setUrl(m_url);
        req.setRawHeader("User-Agent", "alkimia " ALK_VERSION_STRING);
        manager.get(req);

        m_eventLoop = new QEventLoop;
        Q_EMIT m_p->started(m_url);

        if (m_timeout != -1)
            QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));

        result = static_cast<Result>(m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents));
        delete m_eventLoop;
        m_eventLoop = nullptr;
    }
    return result == Result::NoError;
}
#else
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void AlkDownloadEngine::Private::downloadUrlDone(KJob* job)
{
    QString tmpFileName = dynamic_cast<KIO::FileCopyJob*>(job)->destUrl().toLocalFile();
    QUrl url = dynamic_cast<KIO::FileCopyJob*>(job)->srcUrl();

    Result result = NoError;
    if (!job->error()) {
        qDebug() << "Downloaded" << tmpFileName << "from" << url;
        QFile f(tmpFileName);
        if (f.open(QIODevice::ReadOnly)) {
            // TODO Find out the page encoding and convert it to unicode
            QByteArray data = f.readAll();
            Q_EMIT m_p->finished(m_url, data);
            f.close();
        } else {
            Q_EMIT m_p->error(m_url, i18n("Failed to open downloaded file"));
            result = FileNotFoundError;
        }
    } else {
        Q_EMIT m_p->error(m_url, job->errorString());
        result = OtherError;
    }
    m_eventLoop->exit(result);
}

bool AlkDownloadEngine::Private::downloadUrl(const QUrl& url)
{
    // Create a temporary filename (w/o leaving the file on the filesystem)
    // In case the file is still present, the KIO::file_copy operation cannot
    // be performed on some operating systems (Windows).
    auto tmpFile = new QTemporaryFile;
    tmpFile->open();
    auto tmpFileName = QUrl::fromLocalFile(tmpFile->fileName());
    delete tmpFile;

    m_eventLoop = new QEventLoop;
    // TODO add accept language support
    KJob *job = KIO::file_copy(url, tmpFileName, -1, KIO::HideProgressInfo);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(downloadUrlDone(KJob*)));

    Q_EMIT m_p->started(m_url);

    if (m_timeout != -1)
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));

    Result result = static_cast<Result>(m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents));
    delete m_eventLoop;
    m_eventLoop = nullptr;

    return result == NoError;
}
#else // QT_VERSION

// This is simply a placeholder. It is unused but needs to be present
// to make the linker happy (since the declaration of the slot cannot
// be made dependendant on QT_VERSION with the Qt4 moc compiler.
void AlkDownloadEngine::Private::downloadUrlDone(KJob* job)
{
    Q_UNUSED(job);
}

bool AlkDownloadEngine::Private::downloadUrl(const QUrl& url)
{
    Result result = NoError;

    QString tmpFile;
    // TODO add accept language support
    if (KIO::NetAccess::download(url, tmpFile, nullptr)) {
        // kDebug(Private::dbgArea()) << "Downloaded " << tmpFile;
        kDebug(Private::dbgArea()) << "Downloaded" << tmpFile << "from" << url;
        QFile f(tmpFile);
        if (f.open(QIODevice::ReadOnly)) {
            // TODO Find out the page encoding and convert it to unicode
            QByteArray data = f.readAll();
            Q_EMIT m_p->finished(m_url, data);
            f.close();
        } else {
            Q_EMIT m_p->error(m_url, i18n("Failed to open downloaded file"));
            result = FileNotFoundError;
        }
        KIO::NetAccess::removeTempFile(tmpFile);
    } else {
        Q_EMIT m_p->error(m_url, KIO::NetAccess::lastErrorString());
    }
    return result == NoError;
}
#endif // QT_VERSION
#endif // BUILD_WITH_QTNETWORK

#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
void AlkDownloadEngine::Private::slotFinishedJavaScriptEngine(bool ok)
{
    Result result = Result::NoError;
    if (!ok) {
        Q_EMIT m_p->error(m_url, QString());
        result = Result::OtherError;
    } else {
        if (m_type == JavaScriptEngineCSS)
            Q_EMIT m_p->finishedPage(m_url, m_webPage);
        else
            Q_EMIT m_p->finished(m_url, m_webPage->toHtml());
    }
    if (m_eventLoop)
        m_eventLoop->exit(result);
}

bool AlkDownloadEngine::Private::downloadUrlWithJavaScriptEngine(const QUrl &url)
{
    if (!m_webPage) {
        m_webPage = new AlkWebPage;
        m_webPageCreated = true;
    }
    connect(m_webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    connect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(slotFinishedJavaScriptEngine(bool)));
    connect(m_webPage, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));
    m_eventLoop = new QEventLoop;

    int saveTimeout = m_webPage->timeout();
    if (m_timeout != -1) {
        m_webPage->setTimeout(m_timeout);
        QTimer::singleShot(m_timeout, this, SLOT(slotLoadTimeout()));
    }

    m_url = url;
    m_webPage->load(url, m_acceptLanguage);
    Result result = static_cast<Result>(m_eventLoop->exec());
    delete m_eventLoop;
    m_eventLoop = nullptr;

    if (m_timeout != -1)
        m_webPage->setTimeout(saveTimeout);

    if (!m_webPageCreated) {
        disconnect(m_webPage, SIGNAL(loadStarted()), this, SIGNAL(slotLoadStarted()));
        disconnect(m_webPage, SIGNAL(loadFinished(bool)), this, SLOT(slotFinishedJavaScriptEngine(bool)));
        disconnect(m_webPage, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));
    }
    return result == NoError;
}
#endif

AlkDownloadEngine::AlkDownloadEngine(QObject *parent)
    : QObject{parent}
    , d(new Private(this, parent))
{
}

AlkDownloadEngine::~AlkDownloadEngine()
{
    delete d;
}

void AlkDownloadEngine::setWebPage(AlkWebPage *webPage)
{
    d->m_webPage = webPage;
}

void AlkDownloadEngine::setTimeout(int timeout)
{
    d->m_timeout = timeout;
}

int AlkDownloadEngine::timeout()
{
    return d->m_timeout;
}

bool AlkDownloadEngine::downloadUrl(const QUrl &url, Type type)
{
    d->m_type = type;
    switch(type) {
    case DefaultEngine:
        return d->downloadUrl(url);
#if defined(BUILD_WITH_WEBKIT)
    case JavaScriptEngine:
    case JavaScriptEngineCSS:
        return d->downloadUrlWithJavaScriptEngine(url);
#elif defined(BUILD_WITH_WEBENGINE)
    case JavaScriptEngine:
        return d->downloadUrlWithJavaScriptEngine(url);
#endif
    default:
        return false;
    }
    return false;
}

void AlkDownloadEngine::setAcceptedLanguage(const QString &language)
{
    d->m_acceptLanguage = language;
}

#include "alkdownloadengine.moc"
