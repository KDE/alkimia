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

#include "alkwebpage.h"

#if defined(BUILD_WITH_WEBENGINE)
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QUrl>

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
    return QString();
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
    Private()
      : inspector(nullptr)
    {
    }
    ~Private()
    {
        delete inspector;
    }

    void setWebInspectorEnabled(bool enable, QWebPage* page)
    {
      page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, enable);
      delete inspector;
      inspector = nullptr;
      if (enable) {
          inspector = new QWebInspector();
          inspector->setPage(page);
      }
    }

    bool webInspectorEnabled(QWebPage *page)
    {
        return page->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled);
    }
};

AlkWebPage::AlkWebPage(QWidget *parent)
  : QWebView(parent)
  , d(new Private)
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
    d->setWebInspectorEnabled(enable, page());
}

bool AlkWebPage::webInspectorEnabled()
{
    return d->webInspectorEnabled(page());
}

#else

class AlkWebPage::Private
{
public:
};

AlkWebPage::AlkWebPage(QWidget *parent)
  : QWidget(parent)
  , d(new Private)
{
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
    Q_UNUSED(s)
}

QString AlkWebPage::toHtml()
{
    return QString();
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
