/*
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebview.h"

#include "alkdebug.h"
#include "alkwebpage.h"

#include <QEventLoop>
#include <QLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#if defined(ALKIMIA_WEBENGINE)
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

void AlkWebView::setWebPage(AlkWebPage *webPage)
{
    if (auto oldPage = AlkWebView::webPage())
        disconnect(oldPage, nullptr, this, nullptr);

    setPage(dynamic_cast<QWebEnginePage*>(webPage));

    connect(webPage, &AlkWebPage::loadRedirectedTo,
            this, &AlkWebView::loadRedirectedTo,
            Qt::UniqueConnection);
}

AlkWebPage *AlkWebView::webPage()
{
    return dynamic_cast<AlkWebPage*>(page());
}

void AlkWebView::contextMenuEvent(QContextMenuEvent *event)
{
    if (!s_webInspectorEnabled) {
        QWebEngineView::contextMenuEvent(event);
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
    QMenu *menu = createStandardContextMenu();
#else
    QMenu *menu = page()->createStandardContextMenu();
#endif
    const QList<QAction *> actions = menu->actions();
    auto inspectElement = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::InspectElement));
    if (inspectElement == actions.cend()) {
        auto viewSource = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::ViewSource));
        if (viewSource == actions.cend())
            menu->addSeparator();

        QAction *action = new QAction(menu);
        action->setText(i18n("Open inspector in new window"));
        connect(action, &QAction::triggered, [this]() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
            QDesktopServices::openUrl(QString("http://localhost:%1/devtools/page/%2").arg(s_webInspectorPort).arg(page()->devToolsId()));
#else
            Q_UNUSED(this)
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

#elif defined(ALKIMIA_WEBKIT)
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

void AlkWebView::setHtml(const QString &data, const QUrl &baseUrl)
{
    webPage()->setHtml(data, baseUrl);
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

void AlkWebView::setWebPage(AlkWebPage *webPage)
{
    if (auto oldPage = AlkWebView::webPage())
        disconnect(oldPage, nullptr, this, nullptr);

    setPage(dynamic_cast<QWebPage*>(webPage));

    connect(webPage, &AlkWebPage::loadRedirectedTo,
            this, &AlkWebView::loadRedirectedTo,
            Qt::UniqueConnection);
}

AlkWebPage *AlkWebView::webPage()
{
    return dynamic_cast<AlkWebPage*>(page());
}

#else

AlkWebView::AlkWebView(QWidget *parent)
    : QWidget(parent)
{
}

AlkWebView::~AlkWebView()
{
}

void AlkWebView::load(const QUrl &url)
{
    webPage()->load(url, QString());
}

void AlkWebView::setHtml(const QString &data, const QUrl &baseUrl)
{
    webPage()->setHtml(data, baseUrl);
}

void AlkWebView::setWebInspectorEnabled(bool enable)
{
    Q_UNUSED(enable);
}

bool AlkWebView::webInspectorEnabled()
{
    return false;
}

void AlkWebView::setWebPage(AlkWebPage *webPage)
{
    if (m_page == webPage)
        return;

    if (m_page)
        disconnect(m_page, nullptr, this, nullptr);

    auto *layout = qobject_cast<QGridLayout *>(this->layout());
    if (!layout) {
        layout = new QGridLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
    }

    if (m_page)
        layout->removeWidget(m_page);

    m_page = webPage;
    layout->addWidget(webPage);

    connect(webPage, &AlkWebPage::loadFinished, this, &AlkWebView::loadFinished, Qt::UniqueConnection);
    connect(webPage, &AlkWebPage::loadRedirectedTo,
            this, &AlkWebView::loadRedirectedTo,
            Qt::UniqueConnection);
    connect(webPage, &AlkWebPage::loadStarted, this, &AlkWebView::loadStarted, Qt::UniqueConnection);
}

AlkWebPage *AlkWebView::webPage()
{
    return m_page;
}
#endif
