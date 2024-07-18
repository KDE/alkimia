/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"
#include "alknewstuffentry_p.h"

#include "alkdebug.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <KNSCore/EngineBase>
#include <KNSCore/Provider>
#include <KNSCore/ResultsStream>
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <KNSCore/Cache>
#include <knewstuff_version.h>
#include <knscore/engine.h>
#else
#include <knewstuff3/core/cache.h>
#include <knewstuff3/downloadmanager.h>
#define KNEWSTUFF_VERSION 0
#endif

#include <QEventLoop>
#include <QPointer>
#include <QWidget>

class AlkNewStuffEngine::Private : public QObject
{
    Q_OBJECT
public:
    AlkNewStuffEngine *q;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointer<KNSCore::EngineBase> m_engine;
    #define KNS3 KNSCore
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QPointer<KNSCore::Engine> m_engine;
    QSharedPointer<KNSCore::Cache> m_cache;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
#else
    QPointer<KNS3::DownloadManager> m_engine;
    Cache* m_cache;
#endif
    QEventLoop m_loop;

    explicit Private(AlkNewStuffEngine *parent);
    ~Private();

    bool init(const QString &configFile);
    void checkForUpdates();

    const AlkNewStuffEntryList installedEntries();

public Q_SLOTS:
    void slotUpdatesAvailable(const KNS3::Entry::List &entries);
};

AlkNewStuffEngine::Private::Private(AlkNewStuffEngine *parent)
    : q(parent)
    , m_engine(nullptr)
    , m_cache(nullptr)
{
}

AlkNewStuffEngine::Private::~Private()
{
    delete m_engine;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    delete m_cache;
#endif
}

bool AlkNewStuffEngine::Private::init(const QString &configFile)
{
    bool state = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_engine = new KNSCore::EngineBase(this);
    connect(m_engine, &KNSCore::EngineBase::signalProvidersLoaded, this, [this]()
    {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        if (m_wantUpdates) {
            checkForUpdates();
        }
    });
#elif KNEWSTUFF_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_engine = new KNSCore::Engine(this);
    state = m_engine->init(configFile);
    if (!state)
        return false;
    m_cache = m_engine->cache();

    q->connect(m_engine, &KNSCore::Engine::signalErrorCode, q, [](const KNSCore::ErrorCode &, const QString &message, const QVariant &) {
        alkDebug() << message;
    });

    connect(m_engine, &KNSCore::Engine::signalProvidersLoaded, this, [this]()
    {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        m_engine->reloadEntries();
        alkDebug() << "cache" << m_engine->cache() << m_engine->cache()->registry();
        if (m_wantUpdates)
            m_engine->checkForUpdates();
    });

    connect(m_engine, &KNSCore::Engine::signalUpdateableEntriesLoaded, this, [this](const KNSCore::EntryInternal::List &entries)
    {
        alkDebug() << entries.size() << "updates loaded";
        AlkNewStuffEntryList updateEntries;
        toAlkEntryList(updateEntries, entries);
        alkDebug() << updateEntries;
        Q_EMIT q->updatesAvailable(updateEntries);
    });
#else
    m_engine = new KNS3::DownloadManager(configFile, this);
    QFileInfo f(configFile);
    m_cache = new Cache(f.baseName());
    m_cache->readRegistry();
    // no chance get the state
    state = true;

    connect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
            SLOT(slotUpdatesAvailable(KNS3::Entry::List)));
#endif
    return state;
}

void AlkNewStuffEngine::Private::checkForUpdates()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6: no checkforUpdates() - how to proceed ?";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (m_providersLoaded && !m_wantUpdates) {
        m_engine->checkForUpdates();
    } else
        m_wantUpdates = true;
#else
    m_engine->checkForUpdates();
#endif
}

const AlkNewStuffEntryList AlkNewStuffEngine::Private::installedEntries()
{
    AlkNewStuffEntryList result;
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6:";
#else
    if (m_cache)
        toAlkEntryList(result, m_cache->registry());
#endif
    alkDebug() << result;
    return result;
}

void AlkNewStuffEngine::Private::slotUpdatesAvailable(const KNS3::Entry::List &entries)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_UNUSED(entries);
#else
    alkDebug() << entries.size() << "updates loaded";
    AlkNewStuffEntryList updateEntries;
    toAlkEntryList(updateEntries, entries);
    alkDebug() << entries;

    Q_EMIT q->updatesAvailable(updateEntries);
#endif
}

AlkNewStuffEngine::AlkNewStuffEngine(QObject *parent)
    : QObject{parent}
    , d(new Private(this))
{
}

bool AlkNewStuffEngine::init(const QString &configFile)
{
    bool result = d->init(configFile);
    return result;
}

void AlkNewStuffEngine::checkForUpdates()
{
    d->checkForUpdates();
}

AlkNewStuffEntryList AlkNewStuffEngine::installedEntries() const
{
    return d->installedEntries();
}

void AlkNewStuffEngine::reload()
{
    d->m_cache->readRegistry();
}

const char *toString(AlkNewStuffEntry::Status status)
{
    switch(status) {
        case AlkNewStuffEntry::Invalid: return "Invalid";
        case AlkNewStuffEntry::Downloadable: return "Downloadable";
        case AlkNewStuffEntry::Installed: return "Installed";
        case AlkNewStuffEntry::Updateable: return "Updateable";
        case AlkNewStuffEntry::Deleted: return "Deleted";
        case AlkNewStuffEntry::Installing: return "Installing";
        case AlkNewStuffEntry::Updating: return "Updating";
    }
    return "";
}

#include "alknewstuffengine.moc"
