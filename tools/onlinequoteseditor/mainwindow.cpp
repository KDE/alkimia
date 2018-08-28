/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequoteswidget.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QNetworkRequest>
#include <QWebInspector>

class MainWindow::Private
{
public:
    Private()
        : view(nullptr)
        , urlLine(nullptr)
        , quotesWidget(nullptr)
    {
    }

    ~Private()
    {
        delete view;
        delete quotesWidget;
    }
    QWebView *view;
    QLineEdit *urlLine;
    AlkOnlineQuotesWidget *quotesWidget;
};

void MainWindow::slotUrlChanged(const QUrl &url)
{
    d->urlLine->setText(url.toString());
}

void MainWindow::slotEditingFinished()
{
    QUrl url(d->urlLine->text());
    QNetworkRequest request;
    request.setUrl(url);
    if (!d->quotesWidget->acceptLanguage().isEmpty())
        request.setRawHeader("Accept-Language", d->quotesWidget->acceptLanguage().toLocal8Bit());
    d->view->load(request);
}

void MainWindow::slotLanguageChanged(const QString &text)
{
    d->quotesWidget->setAcceptLanguage(text);
    if (!d->urlLine->text().isEmpty())
        slotEditingFinished();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new Private)
    , ui(new Ui::MainWindow)
{
    AlkOnlineQuotesProfileManager &manager = AlkOnlineQuotesProfileManager::instance();

    manager.addProfile(new AlkOnlineQuotesProfile("alkimia", AlkOnlineQuotesProfile::Type::KMyMoney, "alkimia-quotes.knsrc"));
    //manager.addProfile(new AlkOnlineQuotesProfile("local", AlkOnlineQuotesProfile::Type::GHNS, "alkimia-quotes-local.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("skrooge", AlkOnlineQuotesProfile::Type::GHNS, "skrooge-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney", AlkOnlineQuotesProfile::Type::KMyMoney, "kmymoney-quotes.knsrc"));
    ui->setupUi(this);

    QDockWidget *dockWidget = new QDockWidget(tr("Browser"), this);
    d->view = new QWebView;
    connect(d->view, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));

    d->urlLine = new QLineEdit;
    connect(d->urlLine, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));

    d->quotesWidget = new AlkOnlineQuotesWidget;
    d->quotesWidget->setView(d->view);
    setCentralWidget(d->quotesWidget);

    // setup language box
    QComboBox *box = new QComboBox;
    QList<QLocale> allLocales = QLocale::matchingLocales(
            QLocale::AnyLanguage,
            QLocale::AnyScript,
            QLocale::AnyCountry);

    QStringList languages;
    foreach(const QLocale &locale, allLocales) {
        languages.append(locale.uiLanguages());
    }
    languages.sort();
    box->addItems(languages);
    d->quotesWidget->setAcceptLanguage(box->currentText());
    connect(box, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotLanguageChanged(QString)));

    // setup layouts
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(d->urlLine);
    hLayout->addWidget(box);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(hLayout);
    layout->addWidget(d->view);
    QWidget *group = new QWidget;
    group->setLayout(layout);
    dockWidget->setWidget(group);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    // setup inspector
    d->view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebInspector *inspector = new QWebInspector;
    inspector->setPage(d->view->page());
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}
