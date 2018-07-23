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

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequoteswidget.h"

#include <QDockWidget>
#include <QWebInspector>

class MainWindow::Private
{
public:
    QWebView *view;
    QLineEdit *urlLine;
};

void MainWindow::slotUrlChanged(const QUrl &url)
{
    d->urlLine->setText(url.toString());
}

void MainWindow::slotEditingFinished()
{
    d->view->setUrl(QUrl(d->urlLine->text()));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d(new Private),
    ui(new Ui::MainWindow)
{
    AlkOnlineQuotesProfileManager &manager = AlkOnlineQuotesProfileManager::instance();

    manager.addProfile(new AlkOnlineQuotesProfile("alkimia", AlkOnlineQuotesProfile::Type::KMyMoney));
    //manager.addProfile(new AlkOnlineQuotesProfile("onlinequoteseditor", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit.knsrc"));
    //manager.addProfile(new AlkOnlineQuotesProfile("local", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit_local.knsrc"));
    //manager.addProfile(new AlkOnlineQuotesProfile("skrooge", AlkOnlineQuotesProfile::Type::GHNS, "skrooge_unit.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney", AlkOnlineQuotesProfile::Type::KMyMoney));
    AlkOnlineQuoteSource::setProfile(manager.profiles().first());
    ui->setupUi(this);

    QDockWidget *dockWidget = new QDockWidget(tr("Browser"), this);
    d->view = new QWebView;
    connect(d->view, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));

    QVBoxLayout *layout = new QVBoxLayout;
    d->urlLine = new QLineEdit;
    connect(d->urlLine, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
    layout->addWidget(d->urlLine);
    layout->addWidget(d->view);
    QWidget *group = new QWidget;
    group->setLayout(layout);
    dockWidget->setWidget(group);
    addDockWidget(Qt::RightDockWidgetArea, dockWidget);

    // setup inspector
    d->view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebInspector *inspector = new QWebInspector;
    inspector->setPage(d->view->page());

    AlkOnlineQuotesWidget *quotesWidget = new AlkOnlineQuotesWidget;
    quotesWidget->setView(d->view);
    setCentralWidget(quotesWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

