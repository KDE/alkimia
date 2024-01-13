/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffwidget.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"

#include <QApplication>
#include <QPointer>

#include <KAboutData>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QApplication>
    #include <KLocalizedString>

    #define _i18n i18n
    #define LICENCE_GPL KAboutLicense::GPL
    #define CATALOG
    #define aboutName() about.displayName()
#else
    #include <KApplication>
    #include <KCmdLineArgs>
    #include <kmenu.h>

    #undef QStringLiteral
    #define QStringLiteral QByteArray
    #define _i18n ki18n
    #define LICENCE_GPL KAboutData::License_GPL
    #define CATALOG QByteArray("onlinequoteseditor"),
    #define aboutName() about.programName()
#endif

#if defined(Q_OS_WIN) && !defined(BUILD_WITH_KIO)
#include <QNetworkProxyFactory>
#endif

int main(int argc, char *argv[])
{
    KAboutData about(QStringLiteral("onlinequoteseditor"),
                     CATALOG
                     _i18n("Online Quotes Editor"),
                     QStringLiteral("1.0"),
                     _i18n("Editor for online price quotes used by finance applications"),
                     LICENCE_GPL,
                     _i18n("(C) 2018-2023 Ralf Habacker"));

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QApplication app(argc,argv);
#else
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app(true);
#endif

#if defined(Q_OS_WIN) && !defined(BUILD_WITH_KIO)
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia5, "alkimia-quotes.knsrc");
    AlkOnlineQuotesProfileManager::instance().addProfile(&profile);

    QString configFile = profile.hotNewStuffConfigFile();

    AlkNewStuffWidget engine;
    engine.init(configFile);
    engine.showInstallDialog();

    app.exec();
}
