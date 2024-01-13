/*
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESPROFILE_P_H
#define ALKONLINEQUOTESPROFILE_P_H

#include "alkonlinequotesprofile.h"

#ifdef ENABLE_FINANCEQUOTE
#include "alkfinancequoteprocess.h"
#endif

#include <KConfigGroup>

#ifdef ENABLE_FINANCEQUOTE
#include <QApplication>
#endif

#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <KNSCore/Engine>
    #include <KSharedConfig>
    namespace KNS = KNSCore;
#else
    #include <knewstuff3/downloadmanager.h>
    namespace KNS = KNS3;
#endif

class ALK_NO_EXPORT AlkOnlineQuotesProfile::Private : public QObject
{
    Q_OBJECT
public:
    AlkOnlineQuotesProfile *m_p;
    QString m_name;
    QString m_GHNSFile;
    QString m_GHNSFilePath;
    QString m_kconfigFile;
    AlkOnlineQuotesProfileManager *m_profileManager;
    KSharedConfigPtr m_config;
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    KNS::DownloadManager *m_manager = 0;
#else
    KNSCore::Engine *m_engine = 0;
#endif
    Type m_type;
    static QString m_financeQuoteScriptPath;
    static QStringList m_financeQuoteSources;

    bool setupFinanceQuoteScriptPath();

    explicit Private(AlkOnlineQuotesProfile *p);

    ~Private();

    void checkUpdates();

public Q_SLOTS:
    void slotUpdatesFound(const KNS3::Entry::List &updates);

    // to know about finished installations
    void entryStatusChanged(const KNS3::Entry &entry);

    const QStringList quoteSourcesNative();

#ifdef ENABLE_FINANCEQUOTE
    const QStringList quoteSourcesFinanceQuote();
#endif

    const QStringList quoteSourcesSkrooge();

    const QStringList quoteSourcesGHNS();

    const AlkOnlineQuotesProfile::Map defaultQuoteSources();

    /**
     * @brief return data root path
     * @return path
     */
    QString dataRootPath();

    /**
     * @brief return home root path
     * @return path
     */
    QString homeRootPath();

    QString configPath();

    QString dataReadPath();

    QString dataWritePath();
};

#endif // ALKONLINEQUOTESPROFILE_P_H
