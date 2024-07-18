/*
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org
    SPDX-FileCopyrightText: 2018, 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESPROFILE_P_H
#define ALKONLINEQUOTESPROFILE_P_H

#include "alkonlinequotesprofile.h"

#include "alknewstuffengine.h"

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
#include <KSharedConfig>
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
    AlkNewStuffEngine *m_engine = 0;
    KSharedConfigPtr m_config;
    Type m_type;
    AlkNewStuffEntryList m_installedNewStuffEntries;
    static QString m_financeQuoteScriptPath;
    static QStringList m_financeQuoteSources;

    bool setupFinanceQuoteScriptPath();

    explicit Private(AlkOnlineQuotesProfile *p);

    ~Private();

    QString GHNSId(const QString &name);
    QString GHNSFilePath(const QString &name);
    QString GHNSName(const QString &id);

    const QStringList quoteSourcesNative();

#ifdef ENABLE_FINANCEQUOTE
    const QStringList quoteSourcesFinanceQuote();
#endif

    const QStringList quoteSourcesSkrooge();

    const QStringList quoteSourcesGHNS();

    Map quoteSourcesTesting();

    const AlkOnlineQuotesProfile::Map defaultQuoteSources();

    void updateQuoteSources(QStringList &sources);

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

    void reload();

    const AlkNewStuffEntryList &installedNewStuffEntries();

public Q_SLOTS:
    void slotUpdatesAvailable(const AlkNewStuffEntryList &updates);
};

#endif // ALKONLINEQUOTESPROFILE_P_H
