/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"
#include "alknewstuffentry_p.h"

#include "alkdebug.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <KNSCore/Cache>
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
#include <QTimer>
#include <QWidget>

class AlkNewStuffEngine::Private : public QObject
{
    Q_OBJECT
public:
    AlkNewStuffEngine *q;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointer<KNSCore::EngineBase> m_engine;
    #define KNS3 KNSCore
    QSharedPointer<KNSCore::Cache> m_cache;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
    KNSCore::Entry::List m_availableEntries;
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QPointer<KNSCore::Engine> m_engine;
    QSharedPointer<KNSCore::Cache> m_cache;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
    KNSCore::EntryInternal::List m_availableEntries;
#else
    QPointer<KNS3::DownloadManager> m_engine;
    Cache* m_cache;
#endif
    QEventLoop m_loop;

    explicit Private(AlkNewStuffEngine *parent);
    ~Private();

    bool init(const QString &configFile);
    void checkForUpdates();

    const AlkNewStuffEntryList availableEntries();
    const AlkNewStuffEntryList installedEntries();

    bool install(const AlkNewStuffEntry &entry);
    bool uninstall(const AlkNewStuffEntry &entry);

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
    state = m_engine->init(configFile);
    if (!state)
        return false;
    m_cache = m_engine->cache();
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

    q->connect(m_engine, &KNSCore::Engine::signalErrorCode, q, [](const KNSCore::ErrorCode &errorCode, const QString &message, const QVariant &) {
        alkDebug() << "KNSCore::Engine error:" << errorCode << message;
    });

    connect(m_engine, &KNSCore::Engine::signalProvidersLoaded, this, [this]() {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        m_engine->reloadEntries();
        alkDebug() << "cache" << m_engine->cache() << m_engine->cache()->registry();
    });

    connect(m_engine, &KNSCore::Engine::signalEntriesLoaded, this, [this](const KNSCore::EntryInternal::List &entries) {
        m_availableEntries = entries;
        alkDebug() << entries.size() << "entries loaded";
        AlkNewStuffEntryList availableEntries;
        toAlkEntryList(availableEntries, entries);
        alkDebug() << availableEntries;
        Q_EMIT q->entriesAvailable(availableEntries);
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

const AlkNewStuffEntryList AlkNewStuffEngine::Private::availableEntries()
{
    AlkNewStuffEntryList result;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#else
    toAlkEntryList(result, m_availableEntries);

    alkDebug() << result;
#endif
    return result;
}

const AlkNewStuffEntryList AlkNewStuffEngine::Private::installedEntries()
{
    AlkNewStuffEntryList result;
    if (m_cache)
        toAlkEntryList(result, m_cache->registry());

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

bool AlkNewStuffEngine::Private::install(const AlkNewStuffEntry &entry)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6: mussing implementation for installing GHNS entries";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    for (const auto &e : m_availableEntries) {
        if (entry.name != e.name() || entry.status == AlkNewStuffEntry::Installed) {
            continue;
        }
        const auto uniqueId = e.uniqueId();
        bool finished = false;
        bool success = false;
        auto conn = connect(m_engine,
                            &KNSCore::Engine::signalEntryEvent,
                            [this, &finished, &success, &uniqueId](const KNSCore::EntryInternal &entry, KNSCore::EntryInternal::EntryEvent event) {
                                // qDebug() << event << entry.name() << entry.status();
                                if (event == KNSCore::EntryInternal::StatusChangedEvent && entry.status() == KNS3::Entry::Status::Installed
                                    && uniqueId == entry.uniqueId()) {
                                    finished = true;
                                    success = true;

                                    if (m_loop.isRunning()) {
                                        m_loop.quit();
                                    }

                                    qDebug() << "install completed";
                                }
                            });

        QTimer timeout;
        timeout.setSingleShot(true);
        connect(&timeout, &QTimer::timeout, &m_loop, &QEventLoop::quit);
        timeout.start(5000);

        m_engine->install(e);

        // The operation may have completed synchronously.
        if (!finished) {
            m_loop.exec();
        }
        disconnect(conn);

        if (!finished || !success) {
            qDebug() << "timeout installing" << e.name();
            return false;
        }

        return true;
    }
#else
#endif
    return false;
}

bool AlkNewStuffEngine::Private::uninstall(const AlkNewStuffEntry &entry)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6: mussing implementation for installing GHNS entries";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    for (const auto &e : m_availableEntries) {
        if (entry.name != e.name() || (entry.status != AlkNewStuffEntry::Installed && entry.status != AlkNewStuffEntry::Updateable)) {
            continue;
        }
        const auto uniqueId = e.uniqueId();
        bool finished = false;
        bool success = false;
        auto conn = connect(m_engine,
                            &KNSCore::Engine::signalEntryEvent,
                            [this, &finished, &success, &uniqueId](const KNSCore::EntryInternal &entry, KNSCore::EntryInternal::EntryEvent event) {
                                // qDebug() << event << entry.name() << entry.status();
                                if (event == KNSCore::EntryInternal::StatusChangedEvent && entry.status() == KNS3::Entry::Status::Deleted
                                    && uniqueId == entry.uniqueId()) {
                                    finished = true;
                                    success = true;

                                    if (m_loop.isRunning()) {
                                        m_loop.quit();
                                    }

                                    qDebug() << "uninstall completed";
                                }
                            });

        QTimer timeout;
        timeout.setSingleShot(true);
        connect(&timeout, &QTimer::timeout, &m_loop, &QEventLoop::quit);
        timeout.start(10000);

        m_engine->uninstall(e);

        // The operation may have completed synchronously.
        if (!finished) {
            m_loop.exec();
        }

        disconnect(conn);
        qDebug() << "after loop";

        if (!finished || !success) {
            qDebug() << "timeout uninstalling" << e.name();
            return false;
        }

        return true;
    }
#else
    alkDebug() << "FIXME: mussing implementation for installing GHNS entries";
#endif
    return false;
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

AlkNewStuffEntryList AlkNewStuffEngine::availableEntries() const
{
    return d->availableEntries();
}

AlkNewStuffEntryList AlkNewStuffEngine::installedEntries() const
{
    return d->installedEntries();
}

void AlkNewStuffEngine::reload()
{
    d->m_cache->readRegistry();
}

bool AlkNewStuffEngine::install(const AlkNewStuffEntry &entry)
{
    return d->install(entry);
}

bool AlkNewStuffEngine::uninstall(const AlkNewStuffEntry &entry)
{
    return d->uninstall(entry);
}

void AlkNewStuffEngine::setProviderId(const QString &name, const QString &providerId)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    alkDebug() << "FIXME Qt6: mussing implementation for provider id setup";
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    for (const auto &e : d->m_cache->registry()) {
        if (name == e.name()) {
            KNSCore::EntryInternal entry(e);
            entry.setProviderId(providerId);
            d->m_cache->registerChangedEntry(entry);
            d->m_cache->writeRegistry();
        }
    }
#else
    alkDebug() << "FIXME: mussing implementation for provider id setup";
#endif
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
