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

class ALK_EXPORT AlkWebPage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    QString toHtml();
    void setContent(const QString &s);
    QString getFirstElement(const QString &symbol);
    static void setWebInspectorEnabled(bool state);
    static bool webInspectorEnabled();

private:
    class Private;
    Private *d;
};

#elif defined(BUILD_WITH_WEBKIT)

#include <QWebView>

/**
 * The AlkWebPage class provides an interface
 * to a browser component
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPage : public QWebView
{
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    QString toHtml();
    QString getFirstElement(const QString &symbol);
    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();

private:
    class Private;
    Private *d;
};

#else

#include <QTextBrowser>

/**
 * The AlkWebPage class provides an interface
 * to a browser component with javascript support
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker ralf.habacker @freenet.de
 */
class ALK_EXPORT AlkWebPage : public QTextBrowser
{
    Q_OBJECT
public:
    explicit AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    void setHtml(const QString &data, const QUrl &url = QUrl());
    void setUrl(const QUrl &url);
    void setContent(const QString &s);
    QString getFirstElement(const QString &symbol);
    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();
Q_SIGNALS:
    void loadStarted();
    void loadFinished(bool);
    void urlChanged(const QUrl&);

private:
    class Private;
    Private *d;
    QVariant loadResource(int type, const QUrl &name) override;
};
#endif

#endif // ALKWEBPAGE_H
