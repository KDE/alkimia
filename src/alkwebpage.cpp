/*
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebpage.h"

#include "alkwebview.h"

#if defined(BUILD_WITH_WEBENGINE)
#include <klocalizedstring.h>

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QEventLoop>
#include <QMenu>
#include <QPointer>
#include <QTimer>
#include <QUrl>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>

class AlkWebPage::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebPage *q;
    int timeout{-1}; // msec

    explicit Private(AlkWebPage *_q)
        : q(_q)
    {
    }
    Private(const Private& right) = delete;
    Private& operator=(const Private& right) = delete;
};

AlkWebPage::AlkWebPage(QWidget *parent)
  : QWebEnginePage(parent)
  , d(new Private(this))
{
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    profile()->setHttpAcceptLanguage(acceptLanguage);
    setUrl(url);
}

QString AlkWebPage::toHtml()
{
    QString html;
    QPointer <QEventLoop> loop = new QEventLoop;
    QWebEnginePage::toHtml([&html, loop](const QString &result)
        {
            html = result;
            loop->quit();
        }
    );
    if (d->timeout != -1)
        QTimer::singleShot(d->timeout, loop, SLOT(quit()));
    loop->exec();
    return html;
}

QStringList AlkWebPage::getAllElements(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QStringList();
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QString();
}

void AlkWebPage::setTimeout(int timeout)
{
    d->timeout = timeout;
}

int AlkWebPage::timeout()
{
    return d->timeout;
}

bool AlkWebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeRedirect && isMainFrame)
        Q_EMIT loadRedirectedTo(url);
    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

#include "alkwebpage.moc"

#elif defined(BUILD_WITH_WEBKIT)
#include <QWebFrame>
#include <QWebElement>
#include <QWebInspector>
#include <QWebView>
#include <QNetworkRequest>

class AlkWebPage::Private
{
public:
    AlkWebPage *p;
    QNetworkAccessManager *networkAccessManager;
    explicit Private(AlkWebPage *parent)
        : p(parent)
        , networkAccessManager(new QNetworkAccessManager)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,9,0)
        // see https://community.kde.org/Policies/API_to_Avoid#QNetworkAccessManager
        networkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
#endif
        p->setNetworkAccessManager(networkAccessManager);
    }

    ~Private()
    {
        delete networkAccessManager;
    }
};

AlkWebPage::AlkWebPage(QObject *parent)
  : QWebPage(parent)
  , d(new Private(this))
{
    settings()->setAttribute(QWebSettings::JavaEnabled, false);
    settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    settings()->setAttribute(QWebSettings::PluginsEnabled, false);
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    QNetworkRequest request;
    request.setUrl(url);
    if (!acceptLanguage.isEmpty())
        request.setRawHeader("Accept-Language", acceptLanguage.toLocal8Bit());
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (url.query().toLower().contains(QLatin1String("method=post"))) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        mainFrame()->load(request, QNetworkAccessManager::PostOperation, url.query().toUtf8());
#else
    if (url.hasQueryItem(QLatin1String("method")) && url.queryItemValue(QLatin1String("method")).toLower()== QLatin1String("post")) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        mainFrame()->load(request, QNetworkAccessManager::PostOperation);
#endif
    } else
        mainFrame()->load(request);
}

void AlkWebPage::setHtml(const QString &data)
{
    // TODO baseurl
    mainFrame()->setHtml(data);
}

QString AlkWebPage::toHtml()
{
    QWebFrame *frame = mainFrame();
    return frame->toHtml();
}

QStringList AlkWebPage::getAllElements(const QString &symbol)
{
    QStringList result;
    QWebFrame *frame = mainFrame();
    QWebElementCollection elements = frame->findAllElements(symbol);
    for (const auto &e: elements) {
        result.append(e.toPlainText());
    }
    return result;
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    QWebFrame *frame = mainFrame();
    QWebElement element = frame->findFirstElement(symbol);
    return element.toPlainText();
}

#else

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>

class AlkWebPage::Private
{
public:
};

AlkWebPage::AlkWebPage(QWidget *parent)
  : QTextBrowser(parent)
  , d(new Private)
{
    setOpenExternalLinks(false);
    setOpenLinks(false);
    connect(this, SIGNAL(sourceChanged(QUrl)), SIGNAL(loadRedirectedTo(QUrl)));
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    Q_UNUSED(acceptLanguage)
    setSource(url, QTextDocument::HtmlResource);
    Q_EMIT loadStarted();
}

void AlkWebPage::setUrl(const QUrl &url)
{
    load(url, QString());
}

QStringList AlkWebPage::getAllElements(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QStringList();
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QString();
}

QVariant AlkWebPage::loadResource(int type, const QUrl &name)
{
    switch(type) {
    case QTextDocument::HtmlResource:
    case QTextDocument::StyleSheetResource:
        QNetworkAccessManager networkManager;
        QNetworkRequest request;
        request.setUrl(name);
        QNetworkReply* reply = networkManager.get(request);
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
        loop.exec();
        QString data = reply->readAll();
        Q_EMIT loadFinished(true);
        return data;
    }

    return QVariant();
}

#endif
