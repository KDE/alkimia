/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <knewstuff_version.h>
#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
    #include <kns3/downloaddialog.h>
#else
    #include <KNS3/QtQuickDialogWrapper>
#endif
#else
    #include <knewstuff3/downloaddialog.h>
#define KNEWSTUFF_VERSION 0
#endif

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlkOnlineQuotesProfile profile("test", AlkOnlineQuotesProfile::Type::Alkimia5, "alkimia-quotes.knsrc");
    AlkOnlineQuotesProfileManager::instance().addProfile(&profile);

    QString configFile = profile.hotNewStuffConfigFile();

#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog(configFile, this);
    dialog->exec();
    delete dialog;
#elif KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 94, 0)
    KNS3::QtQuickDialogWrapper(configFile).exec();
#else
    auto knsWrapper = new KNS3::QtQuickDialogWrapper(configFile);
    knsWrapper->open();
    QEventLoop loop;
    QObject::connect(knsWrapper, &KNS3::QtQuickDialogWrapper::closed, &loop, &QEventLoop::quit);
    loop.exec();
#endif

    app.exec();
}
