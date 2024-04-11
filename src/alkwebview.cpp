/*
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebview.h"

#include "alkwebpage.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#if defined(BUILD_WITH_WEBENGINE)
#include <klocalizedstring.h>

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QMenu>

// Port used by web inspector, may be converted into a tool setting attribute
static const int s_webInspectorPort{8181};
static bool s_webInspectorEnabled{false};

AlkWebView::AlkWebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(page(), SIGNAL(urlChanged(QUrl)), this, SIGNAL(loadRedirectedTo(QUrl)));
}

AlkWebView::~AlkWebView()
{
}

void AlkWebView::load(const QUrl &url)
{
    QWebEngineView::load(url);
    if (QWebEngineView::url() == url)
        triggerPageAction(QWebEnginePage::Reload);
}

void AlkWebView::setWebInspectorEnabled(bool state)
{
    s_webInspectorEnabled = state;
    if (state)
        qputenv("QTWEBENGINE_REMOTE_DEBUGGING", QByteArray::number(s_webInspectorPort));
    else
        qunsetenv("QTWEBENGINE_REMOTE_DEBUGGING");
}

bool AlkWebView::webInspectorEnabled()
{
    return s_webInspectorEnabled;
}

AlkWebPage *AlkWebView::webPage()
{
    return dynamic_cast<AlkWebPage*>(page());
}

void AlkWebView::setWebPage(AlkWebPage *webPage)
{
    setPage(dynamic_cast<QWebEnginePage*>(webPage));
}

void AlkWebView::contextMenuEvent(QContextMenuEvent *event)
{
    if (!s_webInspectorEnabled) {
        QWebEngineView::contextMenuEvent(event);
        return;
    }
    QMenu *menu = page()->createStandardContextMenu();
    const QList<QAction *> actions = menu->actions();
    auto inspectElement = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::InspectElement));
    if (inspectElement == actions.cend()) {
        auto viewSource = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::ViewSource));
        if (viewSource == actions.cend())
            menu->addSeparator();

        QAction *action = new QAction(menu);
        action->setText(i18n("Open inspector in new window"));
        connect(action, &QAction::triggered, []() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            QDesktopServices::openUrl(QString("http://localhost:%1/devtools/page/%2").arg(webInspectorPort, page()->devToolsId()));
#else
            QDesktopServices::openUrl(QString("http://localhost:%1").arg(s_webInspectorPort));
#endif
        });

        QAction *before(inspectElement == actions.cend() ? nullptr : *inspectElement);
        menu->insertAction(before, action);
    } else {
        (*inspectElement)->setText(i18n("Inspect element"));
    }
    menu->popup(event->globalPos());
}

#elif defined(BUILD_WITH_WEBKIT)
#include <QPointer>
#include <QWebInspector>
#include <QWebView>

static QPointer<QWebInspector> s_webInspector{nullptr};

AlkWebView::AlkWebView(QWidget *parent)
    : QWebView(parent)
{
    page()->settings()->setAttribute(QWebSettings::JavaEnabled, false);
    page()->settings()->setAttribute(QWebSettings::AutoLoadImages, false);
    page()->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    // TODO catch unchanged urls
    connect(this, SIGNAL(urlChanged(QUrl)), this, SIGNAL(loadRedirectedTo(QUrl)));
}

AlkWebView::~AlkWebView()
{
}

void AlkWebView::setWebInspectorEnabled(bool enable)
{
    page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, enable);
    if (enable && !s_webInspector) {
        s_webInspector = new QWebInspector();
        s_webInspector->setPage(page());
    }
}

bool AlkWebView::webInspectorEnabled()
{
    return page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled);
}

AlkWebPage *AlkWebView::webPage()
{
    return dynamic_cast<AlkWebPage*>(page());
}

void AlkWebView::setWebPage(AlkWebPage *webPage)
{
    setPage(dynamic_cast<QWebPage*>(webPage));
}

#else

AlkWebView::AlkWebView(QWidget *parent)
    : QTextBrowser(parent)
{
    setOpenExternalLinks(false);
    setOpenLinks(false);
}

AlkWebView::~AlkWebView()
{
}

void AlkWebView::load(const QUrl &url)
{
    setSource(url, QTextDocument::HtmlResource);
    if (source() == url)
        reload();
    Q_EMIT loadStarted();
}

void AlkWebView::setHtml(const QString &data, const QUrl &baseUrl)
{
    Q_UNUSED(baseUrl);
    QTextBrowser::setHtml(data);
}

void AlkWebView::setWebInspectorEnabled(bool enable)
{
    Q_UNUSED(enable);
}

bool AlkWebView::webInspectorEnabled()
{
    return false;
}

AlkWebPage *AlkWebView::webPage()
{
    return m_page;
}

void AlkWebView::setWebPage(AlkWebPage *webPage)
{
    m_page = webPage;
}

QVariant AlkWebView::loadResource(int type, const QUrl &name)
{
    switch(type) {
    case QTextDocument::HtmlResource:
    case QTextDocument::StyleSheetResource:
        QNetworkAccessManager networkManager;
        QNetworkRequest request;
        QUrl url = name;
        QNetworkReply* reply;
        int counts = 3;
        while(--counts > 0) {
            request.setUrl(url);
            reply = networkManager.get(request);
            QEventLoop loop;
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
            loop.exec();
            if (reply->error() == QNetworkReply::NoError) {
                url = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
                if (!url.isEmpty() && url != reply->url()) {
                    Q_EMIT loadRedirectedTo(reply->url().resolved(url));
                } else {
                    break;
                }
            } else {
                qDebug() << reply->error();
                return QString();
            }
        }
        QString data = reply->readAll();
        Q_EMIT loadFinished(true);
        return data;
    }

    return QVariant();
}

#endif
