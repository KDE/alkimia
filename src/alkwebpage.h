/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

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
    AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    QString toHtml();
    QString getFirstElement(const QString &symbol);
    void setWebInspectorEnabled(bool state);
    bool webInspectorEnabled();

Q_SIGNALS:
    void urlChanged(const QUrl &url);

private:
    class Private;
    Private *d;
};

#elif defined(BUILD_WITH_WEBKIT)

#include <QWebView>

/**
 * The AlkWebPage class provides an interface
 * to a browser component with javascript support
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker <ralf.habacker@freenet.de>
 */
class ALK_EXPORT AlkWebPage : public QWebView
{
public:
    AlkWebPage(QWidget *parent = nullptr);
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

#include <QWidget>

/**
 * The AlkWebPage class provides an interface
 * to a browser component with javascript support
 * It is used for fetching and showing web pages.
 *
 * @author Ralf Habacker <ralf.habacker@freenet.de>
 */
class ALK_EXPORT AlkWebPage : public QWidget
{
public:
    AlkWebPage(QWidget *parent = nullptr);
    virtual ~AlkWebPage();

    QWidget *widget();
    void load(const QUrl &url, const QString &acceptLanguage);
    void setUrl(const QUrl &url);
    void setContent(const QString &s);
    QString toHtml();
    QString getFirstElement(const QString &symbol);
    void setWebInspectorEnabled(bool enable);
    bool webInspectorEnabled();
Q_SIGNALS:
    void loadStarted();
    void loadFinished(bool);

private:
    class Private;
    Private *d;
};
#endif

#endif // ALKWEBPAGE_H
