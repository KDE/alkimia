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
#include "alkwebpage.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QNetworkRequest>
#include <QWebInspector>

class MainWindow::Private
{
public:
    Private()
        : urlLine(nullptr)
        , quotesWidget(nullptr)
    {
    }

    ~Private()
    {
        delete quotesWidget;
    }
    QLineEdit *urlLine;
    AlkOnlineQuotesWidget *quotesWidget;
};

void MainWindow::slotUrlChanged(const QUrl &url)
{
    d->urlLine->setText(url.toString());
}

void MainWindow::slotEditingFinished()
{
    AlkOnlineQuotesProfileManager::instance().webPage()->load(QUrl(d->urlLine->text()), d->quotesWidget->acceptLanguage());
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
    manager.setWebPageEnabled(true);

    manager.addProfile(new AlkOnlineQuotesProfile("no-config-file", AlkOnlineQuotesProfile::Type::None));
    manager.addProfile(new AlkOnlineQuotesProfile("alkimia4", AlkOnlineQuotesProfile::Type::Alkimia4, "alkimia-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("alkimia5", AlkOnlineQuotesProfile::Type::Alkimia5, "alkimia-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("skrooge", AlkOnlineQuotesProfile::Type::None, "skrooge-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney4", AlkOnlineQuotesProfile::Type::KMyMoney4, "kmymoney-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney5", AlkOnlineQuotesProfile::Type::KMyMoney5, "kmymoney-quotes.knsrc"));
    ui->setupUi(this);

    QDockWidget *dockWidget = new QDockWidget(tr("Browser"), this);
    AlkWebPage *webPage = manager.webPage();
    connect(webPage, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));

    d->urlLine = new QLineEdit;
    connect(d->urlLine, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));

    d->quotesWidget = new AlkOnlineQuotesWidget(true, true);
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
    layout->addWidget(webPage);
    QWidget *group = new QWidget;
    group->setLayout(layout);
    dockWidget->setWidget(group);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    webPage->setWebInspectorEnabled(true);
}

MainWindow::~MainWindow()
{
    delete d;
    delete ui;
}
