/*
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKWEBPAGE_H
#define ALKWEBPAGE_H

#include <alkimia/alk_export.h>

#include <QObject>

class QUrl;

#if defined(BUILD_WITH_WEBENGINE)

#include <QWebEnginePage>

/**
 * The AlkWebPage class provides an object for loading
 * web documents in order to offer functionality such
 * as AlkWebView in a widgetless environment.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    void load(const QUrl &url, const QString &acceptLanguage);
    QString toHtml();
    QStringList getAllElements(const QString &symbol);
    QString getFirstElement(const QString &symbol);
    /// Set timeout [ms] for AlkWebPage::toHtml()
    void setTimeout(int timeout = -1);
    int timeout();

Q_SIGNALS:
    void loadRedirectedTo(const QUrl &url);

private:
    class Private;
    Private *d;

    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
};

#elif defined(BUILD_WITH_WEBKIT)

#include <QWebPage>

/**
 * The AlkWebPage class provides an object for loading
 * web documents in order to offer functionality such
 * as AlkWebView in a widgetless environment.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit AlkWebPage(QObject *parent = nullptr);
    virtual ~AlkWebPage();

    void load(const QUrl &url, const QString &acceptLanguage);
    void setHtml(const QString &data);
    QString toHtml();
    QStringList getAllElements(const QString &symbol);
    QString getFirstElement(const QString &symbol);
    void setTimeout(int timeout = -1) { Q_UNUSED(timeout) }
    int timeout() { return -1; }

Q_SIGNALS:
    void loadRedirectedTo(const QUrl&);

private:
    class Private;
    Private *d;
};

#else

#include <QTextBrowser>

/**
 * The AlkWebPage class provides an object to load
 * and view web documents to provide functionality
 * like AlkWebView in a widget-less environment.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPage : public QTextBrowser
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    void load(const QUrl &url, const QString &acceptLanguage);
    void setUrl(const QUrl &url);
    QStringList getAllElements(const QString &symbol);
    QString getFirstElement(const QString &symbol);
    void setTimeout(int timeout = -1) { Q_UNUSED(timeout) }
    int timeout() { return -1; }

Q_SIGNALS:
    void loadStarted();
    void loadFinished(bool);
    void loadRedirectedTo(const QUrl&);

private:
    class Private;
    Private *d;
    QVariant loadResource(int type, const QUrl &name) override;
};
#endif

#endif // ALKWEBPAGE_H
