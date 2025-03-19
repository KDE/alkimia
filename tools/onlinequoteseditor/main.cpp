/*
    SPDX-FileCopyrightText: 2018-2024 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include "alkapplication.h"

#include <KAboutData>
#include <KHelpMenu>
#include <KLocalizedString>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <KMenu>
#endif

#include <QApplication>
#include <QMenuBar>

#if defined(Q_OS_WIN)
#include <QNetworkProxyFactory>
#endif

void checkAndSetVisible(QAction *action, bool state)
{
    if (action)
        action->setVisible(state);
}

int main(int argc, char **argv)
{
    AlkApplication app(argc, argv);

    AlkAboutData about(QStringLiteral("onlinequoteseditor"),
                       "Online Quotes Editor",
                       QStringLiteral("1.0"),
                       "Editor for online price quotes used by finance applications",
                       AlkAboutData::License_GPL,
                       "(C) 2018-2024 Ralf Habacker");

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("onlinequoteseditor5")));

#if defined(Q_OS_WIN)
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    MainWindow w;

    KHelpMenu helpMenu(&w, about.shortDescription());
    helpMenu.menu();
    checkAndSetVisible(helpMenu.action(KHelpMenu::menuHelpContents), false);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    checkAndSetVisible(helpMenu.action(KHelpMenu::menuReportBug), false);
#endif
    checkAndSetVisible(helpMenu.action(KHelpMenu::menuSwitchLanguage), true);
    helpMenu.action(KHelpMenu::menuAboutApp)->setText(i18n("&About %1", about.displayName()));
    w.menuBar()->addMenu(static_cast<QMenu*>(helpMenu.menu()));

    w.show();
    return app.exec();
}
