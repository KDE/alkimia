/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequoteswidget.h"
#include "alkwebpage.h"

#include <QComboBox>
#include <QDockWidget>
#include <QLineEdit>
#include <QUrl>

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
    Ui::MainWindow ui;
};

void MainWindow::slotUrlChanged(const QUrl &url)
{
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    d->urlLine->setText(url.toString());
#else
    Q_UNUSED(url)
#endif
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
    , ApplicationSettings(this, false)
    , d(new Private)
{
    AlkOnlineQuotesProfileManager &manager = AlkOnlineQuotesProfileManager::instance();

    manager.addProfile(new AlkOnlineQuotesProfile("no-config-file", AlkOnlineQuotesProfile::Type::None));
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    manager.addProfile(new AlkOnlineQuotesProfile("alkimia4", AlkOnlineQuotesProfile::Type::Alkimia4, "alkimia-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("skrooge4", AlkOnlineQuotesProfile::Type::Skrooge4, "skrooge-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney4", AlkOnlineQuotesProfile::Type::KMyMoney4, "kmymoney-quotes.knsrc"));
#else
    manager.addProfile(new AlkOnlineQuotesProfile("alkimia5", AlkOnlineQuotesProfile::Type::Alkimia5, "alkimia-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("skrooge5", AlkOnlineQuotesProfile::Type::Skrooge5, "skrooge-quotes.knsrc"));
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney5", AlkOnlineQuotesProfile::Type::KMyMoney5, "kmymoney-quotes.knsrc"));
#endif
#ifdef ENABLE_FINANCEQUOTE
    manager.addProfile(new AlkOnlineQuotesProfile("Finance::Quote", AlkOnlineQuotesProfile::Type::Script));
#endif
    d->ui.setupUi(this);
    d->quotesWidget = new AlkOnlineQuotesWidget(true, true);

    QDockWidget *profilesWidget = new QDockWidget(i18n("Profiles"), this);
    profilesWidget->setObjectName("profilesDockWidget");
    profilesWidget->setWidget(d->quotesWidget->profilesWidget());
    addDockWidget(Qt::LeftDockWidgetArea, profilesWidget);

    QDockWidget *profileDetailsWidget = new QDockWidget(i18n("Profile details"), this);
    profileDetailsWidget->setObjectName("profileDetailsDockWidget");
    profileDetailsWidget->setWidget(d->quotesWidget->profileDetailsWidget());
    addDockWidget(Qt::RightDockWidgetArea, profileDetailsWidget);

    QDockWidget *onlineQuotesWidget = new QDockWidget(i18n("Online quotes"), this);
    onlineQuotesWidget->setObjectName("onlineQuotesDockWidget");
    onlineQuotesWidget->setWidget(d->quotesWidget->onlineQuotesWidget());
    addDockWidget(Qt::LeftDockWidgetArea, onlineQuotesWidget);

    QDockWidget *debugWidget = new QDockWidget(i18n("Debug"), this);
    debugWidget->setObjectName("debugDockWidget");
    debugWidget->setWidget(d->quotesWidget->debugWidget());
    addDockWidget(Qt::LeftDockWidgetArea, debugWidget);

    QDockWidget *quoteDetailsWidget = new QDockWidget(i18n("Quote details"), this);
    quoteDetailsWidget->setObjectName("quoteDetailsDockWidget");
    quoteDetailsWidget->setWidget(d->quotesWidget->quoteDetailsWidget());
    addDockWidget(Qt::RightDockWidgetArea, quoteDetailsWidget);

    connect(&AlkOnlineQuotesProfileManager::instance(), SIGNAL(updateAvailable(const QString &, const QString &)),
            this, SLOT(slotUpdateAvailable(const QString &, const QString &)));

    manager.setWebPageEnabled(true);
    QDockWidget *browserWidget = new QDockWidget(i18n("Browser"), this);
    browserWidget->setObjectName("browserDockWidget");
    AlkWebPage *webPage = manager.webPage();

    QHBoxLayout *hLayout = new QHBoxLayout;
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
    // setup url line
    d->urlLine = new QLineEdit;
    connect(d->urlLine, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
    hLayout->addWidget(d->urlLine);

    // setup language box
    QComboBox *box = new QComboBox;
    QList<QLocale> allLocales = QLocale::matchingLocales(
            QLocale::AnyLanguage,
            QLocale::AnyScript,
            QLocale::AnyCountry);

    QStringList languages;
    for (const QLocale &locale : allLocales) {
        languages.append(locale.uiLanguages());
    }
    languages.sort();
    box->addItems(languages);
    d->quotesWidget->setAcceptLanguage(box->currentText());
    connect(box, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotLanguageChanged(QString)));
    hLayout->addWidget(box);

    webPage->setWebInspectorEnabled(true);
    connect(webPage, SIGNAL(urlChanged(QUrl)), this, SLOT(slotUrlChanged(QUrl)));
#endif
    // setup browser window
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(hLayout);
    layout->addWidget(webPage->widget());
    QWidget *group = new QWidget;
    group->setLayout(layout);
    browserWidget->setWidget(group);
    addDockWidget(Qt::RightDockWidgetArea, browserWidget);

    setCentralWidget(nullptr);

    readPositionSettings();
}

MainWindow::~MainWindow()
{
    delete d;
}

void MainWindow::slotUpdateAvailable(const QString &profile, const QString &name)
{
    QString s = statusBar()->currentMessage();
    if (!s.isEmpty())
        s += " - ";
    s += i18n("Update for '%1' available in profile '%2'", name, profile);
    statusBar()->showMessage(s);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writePositionSettings();
    QMainWindow::closeEvent(event);
}
