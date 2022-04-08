/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebpage.h"

#if defined(BUILD_WITH_WEBENGINE)
#include <QEventLoop>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QUrl>

class AlkWebPage::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebPage *q;

    Private(AlkWebPage *_q) : q(_q) {}
    void slotUrlChanged(const QUrl &url)
    {
        // This workaround is necessary because QWebEnginePage::urlChanged()
        // returns the html content set with setContent() as url.
        if (url.scheme().startsWith("http"))
            emit q->urlChanged(url);
        else
            emit q->urlChanged(QUrl());
    }
};

AlkWebPage::AlkWebPage(QWidget *parent)
  : QWebEnginePage(parent)
  , d(new Private(this))
{
    connect(this, &QWebEnginePage::urlChanged, d, &Private::slotUrlChanged);
}

AlkWebPage::~AlkWebPage()
{
    delete d;
}

QWidget *AlkWebPage::widget()
{
    if (!view())
        setView(new QWebEngineView);
    return view();
}

void AlkWebPage::load(const QUrl &url, const QString &acceptLanguage)
{
    Q_UNUSED(acceptLanguage)

    setUrl(url);
}

QString AlkWebPage::toHtml()
{
    QString html;
    QEventLoop loop;
    QWebEnginePage::toHtml([&html, &loop](const QString &result)
        {
            html = result;
            loop.quit();
        }
    );
    loop.exec();
    return html;
}

void AlkWebPage::setContent(const QString &s)
{
    setHtml(s);
}

QString AlkWebPage::getFirstElement(const QString &symbol)
{
    Q_UNUSED(symbol)

    return QString();
}

void AlkWebPage::setWebInspectorEnabled(bool state)
{
    Q_UNUSED(state)
}

bool AlkWebPage::webInspectorEnabled()
{
    return false;
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
    Private(AlkWebPage *parent)
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
    QWebView::load(request);
}

QString AlkWebPage::toHtml()
{
    QWebFrame *frame = page()->mainFrame();
    return frame->toHtml();
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
    Q_UNUSED(url)
    Q_UNUSED(acceptLanguage)
}

void AlkWebPage::setUrl(const QUrl &url)
{
    Q_UNUSED(url)
}

void AlkWebPage::setContent(const QString &s)
{
    setHtml(s);
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

#endif
