/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebpage.h"

#if defined(BUILD_WITH_WEBENGINE)
#include <klocalizedstring.h>

#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QEventLoop>
#include <QMenu>
#include <QUrl>
#include <QWebEnginePage>
#include <QWebEngineView>

// Port used by web inspector, may be converted into a tool setting attribute
static const int s_webInspectorPort{8181};
static bool s_webInspectorEnabled{false};

class AlkWebView: public QWebEngineView
{
    Q_OBJECT
public:
    AlkWebPage *q;

    explicit AlkWebView(AlkWebPage *parent)
        : QWebEngineView()
        , q(parent)
    {}

    void contextMenuEvent(QContextMenuEvent *event) override
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
};

class AlkWebPage::Private : public QObject
{
    Q_OBJECT
public:
    AlkWebPage *q;
    bool inspectorEnabled{false};

    explicit Private(AlkWebPage *_q)
        : q(_q)
    {
    }
    Private(const Private& right) = delete;
    Private& operator=(const Private& right) = delete;

    void slotUrlChanged(const QUrl &url)
    {
        // This workaround is necessary because QWebEnginePage::urlChanged()
        // returns the html content set with setContent() as url.
        if (url.scheme().startsWith("http"))
            Q_EMIT q->urlChanged(url);
        else
            Q_EMIT q->urlChanged(QUrl());
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (!view())
        setView(new AlkWebView(this));
    return view();
#else
    return QWebEngineView::forPage(this);
#endif
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
    s_webInspectorEnabled = state;
    if (state)
        qputenv("QTWEBENGINE_REMOTE_DEBUGGING", QByteArray::number(s_webInspectorPort));
    else
        qunsetenv("QTWEBENGINE_REMOTE_DEBUGGING");
}

bool AlkWebPage::webInspectorEnabled()
{
    return s_webInspectorEnabled;
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
    connect(this, SIGNAL(sourceChanged(QUrl)), SIGNAL(urlChanged(QUrl)));
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
