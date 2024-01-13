/*
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesprofile_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QDebug>
#include <QRegularExpression>
#else
#include <KConfig>
#include <KDebug>
#include <KGlobal>
#include <KStandardDirs>
#endif

// define static members
QString AlkOnlineQuotesProfile::Private::m_financeQuoteScriptPath;
QStringList AlkOnlineQuotesProfile::Private::m_financeQuoteSources;

bool AlkOnlineQuotesProfile::Private::setupFinanceQuoteScriptPath()
{
    if (m_financeQuoteScriptPath.isEmpty()) {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        m_financeQuoteScriptPath =
                QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("alkimia%1/misc/financequote.pl").arg(TARGET_SUFFIX));
#else
        m_financeQuoteScriptPath = KGlobal::dirs()->findResource("appdata",
                                                                 QString("misc/financequote.pl"));
#endif
    }
    return !m_financeQuoteScriptPath.isEmpty();
}

AlkOnlineQuotesProfile::Private::Private(AlkOnlineQuotesProfile *p)
    : m_p(p)
    , m_profileManager(0)
    , m_engine(new AlkNewStuffEngine)
    , m_config(0)
    , m_type(Type::Undefined)
{
#ifdef ENABLE_FINANCEQUOTE
    setupFinanceQuoteScriptPath();
#endif
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    connect(m_engine, SIGNAL(updatesAvailable(AlkNewStuffEntryList)), this,
            SLOT(slotUpdatesAvailable(AlkNewStuffEntryList)));
#else
    connect(m_engine, &AlkNewStuffEngine::updatesAvailable, this,
            &AlkOnlineQuotesProfile::Private::slotUpdatesAvailable);
#endif
}

AlkOnlineQuotesProfile::Private::~Private()
{
    delete m_engine;
}

const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesNative()
{
    //KSharedConfigPtr kconfig = KGlobal::config();
    auto kconfig = KSharedConfig::openConfig(m_kconfigFile, KConfig::SimpleConfig);
    QStringList groups = kconfig->groupList();

    QStringList::Iterator it;
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QRegExp onlineQuoteSource(QString("^Online-Quote-Source-(.*)$"));

    // get rid of all 'non online quote source' entries
    for (it = groups.begin(); it != groups.end(); it = groups.erase(it)) {
        if (onlineQuoteSource.indexIn(*it) >= 0) {
            // Insert the name part
            it = groups.insert(it, onlineQuoteSource.cap(1));
            ++it;
        }
    }
#else
    QRegularExpression onlineQuoteSource(QLatin1String("^Online-Quote-Source-(.*)$"));

    // get rid of all 'non online quote source' entries
    for (it = groups.begin(); it != groups.end(); it = groups.erase(it)) {
        const auto match = onlineQuoteSource.match(*it);
        if (match.hasMatch()) {
            // Insert the name part
            it = groups.insert(it, match.captured(1));
            ++it;
        }
    }
#endif
    // Set up each of the default sources.  These are done piecemeal so that
    // when we add a new source, it's automatically picked up. And any changes
    // are also picked up.
    QMap<QString, AlkOnlineQuoteSource> defaults = defaultQuoteSources();
    QMap<QString, AlkOnlineQuoteSource>::iterator it_source = defaults.begin();
    while (it_source != defaults.end()) {
        if (!groups.contains((*it_source).name())) {
            groups += (*it_source).name();
            (*it_source).write();
            kconfig->sync();
        }
        ++it_source;
    }

    return groups;
}

#ifdef ENABLE_FINANCEQUOTE
const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesFinanceQuote()
{
    if (m_financeQuoteSources.empty()) { // run the process one time only
        // since this is a static function it can be called without constructing an object
        // so we need to make sure that m_financeQuoteScriptPath is properly initialized
        if (setupFinanceQuoteScriptPath()) {
            AlkFinanceQuoteProcess testList;
            testList.testLaunch(m_financeQuoteScriptPath);
            while (!testList.isFinished()) {
                qApp->processEvents();
            }

            if (testList.exitCode() == 0) {
                AlkFinanceQuoteProcess getList;
                getList.launch(m_financeQuoteScriptPath);
                while (!getList.isFinished()) {
                    qApp->processEvents();
                }
                m_financeQuoteSources = getList.getSourceList();
            }
        }
    }
    return m_financeQuoteSources;
}
#endif

const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesSkrooge()
{
    return quoteSourcesGHNS();
}

const QStringList AlkOnlineQuotesProfile::Private::quoteSourcesGHNS()
{
    QStringList sources;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QStringList resources;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, m_GHNSFilePath, QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString& dir, dirs) {
        const QStringList fileNames = QDir(dir).entryList(QStringList() << QStringLiteral("*.txt"));
        Q_FOREACH (const QString& file, fileNames) {
            resources.append(dir + '/' + file);
        }
    }
#else
    const QString filename = QString("%1/*.txt").arg(m_GHNSFilePath);
    const QStringList resources = KStandardDirs().findAllResources("data", filename);
#endif
    for (const QString &file : resources) {
        QFileInfo f(file);
        QString file2 = f.completeBaseName();
        AlkOnlineQuoteSource source(file2, m_p);
        if (source.isEmpty()) {
            qDebug() << "skipping" << file;
            continue;
        }
        if (!sources.contains(file2)) {
            qDebug() << "adding quote source" << file;
            sources.push_back(file2);
        }
    }

    return sources;
}

const AlkOnlineQuotesProfile::Map AlkOnlineQuotesProfile::Private::defaultQuoteSources()
{
    QMap<QString, AlkOnlineQuoteSource> result;

    // Use fx-rate.net as the standard currency exchange rate source until
    // we have the capability to use more than one source. Use a neutral
    // name for the source.

    switch (m_p->type()) {
    case AlkOnlineQuotesProfile::Type::None:
    case AlkOnlineQuotesProfile::Type::Alkimia4:
    case AlkOnlineQuotesProfile::Type::Alkimia5: {
        AlkOnlineQuoteSource source(AlkOnlineQuoteSource::defaultCurrencyQuoteSource("Alkimia Currency"));
        source.setProfile(m_p);
        result[source.name()] = source;
#if defined(BUILD_WITH_WEBKIT) || defined(BUILD_WITH_WEBENGINE)
        AlkOnlineQuoteSource source2(AlkOnlineQuoteSource::defaultCurrencyQuoteSource("Alkimia Currency.webkit"));
        source2.setProfile(m_p);
        result[source2.name()] = source2;
#endif
        break;
    }
    default:
        break;
    }
    return result;
}

QString AlkOnlineQuotesProfile::Private::dataRootPath()
{
    return QLibraryInfo::location(QLibraryInfo::PrefixPath) + "/share";
}

QString AlkOnlineQuotesProfile::Private::homeRootPath()
{
    if (m_type == Type::KMyMoney5 || m_type == Type::Alkimia5 || m_type == Type::Skrooge5)
        return QDir::homePath();
    else if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4) {
#ifdef Q_OS_WIN
        return qgetenv("APPDATA");
#else
        return QDir::homePath();
#endif
    } else {
        return QString();
    }
}

QString AlkOnlineQuotesProfile::Private::configPath()
{
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    if (m_type == Type::KMyMoney5)
        return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
    else if(m_type == Type::Alkimia5 || m_type == Type::Skrooge5)
        return QString("%1/.config").arg(homeRootPath());
    else
#endif
        if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4)
            return QString("%1/.kde4/share/config").arg(homeRootPath());
    return
            QString();
}

QString AlkOnlineQuotesProfile::Private::dataReadPath()
{
    if (m_type == Type::KMyMoney5 || m_type == Type::Alkimia5 || m_type == Type::Skrooge5)
        return dataRootPath();
    else if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4)
        return QString("%1/kde4/apps").arg(dataRootPath());
    return
            QString();
}

QString AlkOnlineQuotesProfile::Private::dataWritePath()
{
    if (m_type == Type::KMyMoney5 || m_type == Type::Alkimia5 || m_type == Type::Skrooge5)
        return QString("%1/.local/share").arg(homeRootPath());
    else if (m_type == Type::KMyMoney4 || m_type == Type::Alkimia4 || m_type == Type::Skrooge4)
        return QString("%1/.kde4/share/apps").arg(homeRootPath());
    return
            QString();
}

void AlkOnlineQuotesProfile::Private::slotUpdatesAvailable(const AlkNewStuffEntryList &updates)
{
    for (auto &entry : updates) {
        qDebug() << "update available in profile" << m_p->name() << "for"
                 << entry.name << entry.version << entry.id << entry.category
                 << entry.providerId;
        Q_EMIT m_p->updateAvailable(m_p->name(), entry.name);
    }
}
