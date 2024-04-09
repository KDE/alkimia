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

// Port used by web inspector, may be converted into a tool setting attribute
static const int s_webInspectorPort{8181};
static bool s_webInspectorEnabled{false};


class AlkWebPage::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebPage *q;
    bool inspectorEnabled{false};
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

QWidget *AlkWebPage::widget()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!view())
        setView(new AlkWebView);
    return view();
#else
    return QWebEngineView::forPage(this);
#endif
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

void AlkWebPage::setContent(const QString &s)
{
    setHtml(s);
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

void AlkWebPage::setWebInspectorEnabled(bool state)
{
    s_webInspectorEnabled = state;
    if (state)
        qputenv("QTWEBENGINE_REMOTE_DEBUGGING", QByteArray::number(s_webInspectorPort));
    else
        qunsetenv("QTWEBENGINE_REMOTE_DEBUGGING");
}

int AlkWebPage::timeout()
{
    return d->timeout;
}

bool AlkWebPage::webInspectorEnabled()
{
    return s_webInspectorEnabled;
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
    QWebInspector *inspector;
    AlkWebPage *p;
    QNetworkAccessManager *networkAccessManager;
    explicit Private(AlkWebPage *parent)
      : inspector(nullptr),
        p(parent),
        networkAccessManager(new QNetworkAccessManager)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,9,0)
        // see https://community.kde.org/Policies/API_to_Avoid#QNetworkAccessManager
        networkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
#endif
        p->page()->setNetworkAccessManager(networkAccessManager);
    }

    ~Private()
    {
        p->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
        if (inspector)
            inspector->setPage(nullptr);
        delete inspector;
        delete networkAccessManager;
    }

    void setWebInspectorEnabled(bool enable)
    {
        p->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, enable);
        if (enable && !inspector) {
            inspector = new QWebInspector();
            inspector->setPage(p->page());
        }
    }

    bool webInspectorEnabled()
    {
        return p->page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled);
    }
};

AlkWebPage::AlkWebPage(QWidget *parent)
  : QWebView(parent)
  , d(new Private(this))
{
    page()->settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page()->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page()->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

QWidget *AlkWebPage::widget()
{
    return this;
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
        QWebView::load(request, QNetworkAccessManager::PostOperation, url.query().toUtf8());
#else
    if (url.hasQueryItem(QLatin1String("method")) && url.queryItemValue(QLatin1String("method")).toLower()== QLatin1String("post")) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QWebView::load(request, QNetworkAccessManager::PostOperation);
#endif
    } else
        QWebView::load(request);
}

QString AlkWebPage::toHtml()
{
    QWebFrame *frame = page()->mainFrame();
    return frame->toHtml();
}

QStringList AlkWebPage::getAllElements(const QString &symbol)
{
    QStringList result;
    QWebFrame *frame = page()->mainFrame();
    QWebElementCollection elements = frame->findAllElements(symbol);
    for (const auto &e: elements) {
        result.append(e.toPlainText());
    }
    return result;
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    QWebFrame *frame = page()->mainFrame();
    QWebElement element = frame->findFirstElement(symbol);
    return element.toPlainText();
}

void AlkWebPage::setWebInspectorEnabled(bool enable)
{
    d->setWebInspectorEnabled(enable);
}

bool AlkWebPage::webInspectorEnabled()
{
    return d->webInspectorEnabled();
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

QWidget *AlkWebPage::widget()
{
    return this;
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    Q_UNUSED(acceptLanguage)
    setSource(url, QTextDocument::HtmlResource);
    Q_EMIT loadStarted();
}

void AlkWebPage::setHtml(const QString &data, const QUrl &url)
{
    Q_UNUSED(url);
    QTextBrowser::setHtml(data);
}

void AlkWebPage::setUrl(const QUrl &url)
{
    load(url, QString());
}

void AlkWebPage::setContent(const QString &s)
{
    QTextBrowser::setHtml(s);
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

void AlkWebPage::setWebInspectorEnabled(bool enable)
{
    Q_UNUSED(enable)
}

bool AlkWebPage::webInspectorEnabled()
{
    return false;
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
