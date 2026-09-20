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
#include <KNSCore/SearchRequest>
#include <KNSCore/Transaction>
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
    bool m_updateCheckPending{false};
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QPointer<KNSCore::Engine> m_engine;
    QSharedPointer<KNSCore::Cache> m_cache;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
    KNSCore::EntryInternal::List m_availableEntries;
#else
    QPointer<KNS3::DownloadManager> m_engine;
    Cache* m_cache;
    KNS3::Entry::List m_availableEntries;
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool waitForTransaction(KNSCore::Transaction *transaction, KNSCore::Entry::Status expectedStatus, int timeout);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    bool performOperation(const KNSCore::EntryInternal &entry, std::function<void()> operation, KNS3::Entry::Status expectedStatus, int milliSecondsTimeout);
#endif
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
    connect(m_engine, &KNSCore::EngineBase::signalProvidersLoaded, this, [this]() {
        alkDebug() << "providers loaded";
        m_providersLoaded = true;
        if (m_wantUpdates) {
            checkForUpdates();
        }
    });

    connect(m_engine, &KNSCore::EngineBase::signalErrorCode, this, [](const KNSCore::ErrorCode::ErrorCode errorCode, const QString &message, const QVariant &) {
        alkDebug() << "KNSCore::EngineBase error:" << errorCode << message;
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
    if (!m_providersLoaded) {
        m_wantUpdates = true;
        return;
    }

    if (m_updateCheckPending) {
        return;
    }

    m_updateCheckPending = true;

#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    KNSCore::SearchRequest request(KNSCore::SortMode::Downloads, KNSCore::Filter::Updates);
#else
    KNSCore::SearchRequest request;
    request.setFilter(KNSCore::Filter::Updates);
#endif

    KNSCore::ResultsStream *stream = m_engine->search(request);

    auto *entries = new KNSCore::Entry::List;

    connect(stream, &KNSCore::ResultsStream::entriesFound, this, [entries](const KNSCore::Entry::List &found) {
        entries->append(found);
    });

    connect(stream, &KNSCore::ResultsStream::finished, this, [this, entries]() {
        alkDebug() << entries->size() << "updates loaded";

        AlkNewStuffEntryList updateEntries;
        toAlkEntryList(updateEntries, *entries);

        delete entries;

        m_updateCheckPending = false;
        m_wantUpdates = false;

        alkDebug() << "update from KNSCore results stream" << updateEntries;

        Q_EMIT q->updatesAvailable(updateEntries);
    });

    stream->fetch();
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool AlkNewStuffEngine::Private::waitForTransaction(KNSCore::Transaction *transaction, KNSCore::Entry::Status expectedStatus, int timeout)
{
    bool success = false;
    QEventLoop loop;

    connect(transaction, &KNSCore::Transaction::signalEntryEvent, &loop, [&](const KNSCore::Entry &entry, KNSCore::Entry::EntryEvent event) {
        if (event == KNSCore::Entry::StatusChangedEvent && entry.status() == expectedStatus) {
            success = true;
            loop.quit();
        }
    });

    connect(transaction, &KNSCore::Transaction::signalErrorCode, &loop, [&](KNSCore::ErrorCode::ErrorCode, const QString &message, const QVariant &) {
        qDebug() << "KNewStuff transaction failed:" << message;
        loop.quit();
    });

    QTimer timer;
    timer.setSingleShot(true);

    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(timeout);

    if (!transaction->isFinished()) {
        loop.exec();
    }

    return success;
}
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
bool AlkNewStuffEngine::Private::performOperation(const KNSCore::EntryInternal &entry,
                                                  std::function<void()> operation,
                                                  KNS3::Entry::Status expectedStatus,
                                                  int milliSecondsTimeout)
{
    const auto uniqueId = entry.uniqueId();
    bool finished = false;

    const auto conn = connect(m_engine,
                              &KNSCore::Engine::signalEntryEvent,
                              [this, &finished, uniqueId, expectedStatus](const KNSCore::EntryInternal &entry, KNSCore::EntryInternal::EntryEvent event) {
                                  if (event == KNSCore::EntryInternal::StatusChangedEvent && entry.status() == expectedStatus && uniqueId == entry.uniqueId()) {
                                      finished = true;

                                      if (m_loop.isRunning()) {
                                          m_loop.quit();
                                      }
                                  }
                              });

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, &m_loop, &QEventLoop::quit);
    timeoutTimer.start(milliSecondsTimeout);

    operation();

    // The operation may have completed synchronously.
    if (!finished) {
        m_loop.exec();
    }

    disconnect(conn);
    return finished;
}
#endif

bool AlkNewStuffEngine::Private::install(const AlkNewStuffEntry &entry)
{
    bool result = true;
    for (const auto &e : m_availableEntries) {
        if (entry.name != e.name() || entry.status == AlkNewStuffEntry::Installed) {
            continue;
        }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto *transaction = KNSCore::Transaction::installLatest(m_engine, e);
        bool success = waitForTransaction(transaction, KNSCore::Entry::Installed, 5000);
        delete transaction;

        if (!success)
            result = false;
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (!performOperation(
                e,
                [this, &e] {
                    m_engine->install(e);
                },
                KNS3::Entry::Status::Installed,
                5000)) {
            qDebug() << "timeout installing" << e.name();
            result = false;
        }
#else
        m_engine->installEntry(e);
#endif
    }
    return result;
}

bool AlkNewStuffEngine::Private::uninstall(const AlkNewStuffEntry &entry)
{
    bool result = true;
    for (const auto &e : m_availableEntries) {
        if (entry.name != e.name() || (entry.status != AlkNewStuffEntry::Installed && entry.status != AlkNewStuffEntry::Updateable)) {
            continue;
        }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        auto *transaction = KNSCore::Transaction::uninstall(m_engine, e);

        bool success = waitForTransaction(transaction, KNSCore::Entry::Deleted, 10000);
        delete transaction;

        if (!success)
            result = false;
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (!performOperation(
                e,
                [this, &e] {
                    m_engine->uninstall(e);
                },
                KNS3::Entry::Status::Deleted,
                5000)) {
            qDebug() << "timeout uninstalling" << e.name();
            result = false;
        }
#else
        m_engine->installEntry(e);
#endif
    }

    return result;
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    for (const auto &e : d->m_cache->registry()) {
        if (name == e.name()) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            KNSCore::Entry entry(e);
#else
            KNSCore::EntryInternal entry(e);
#endif
            entry.setProviderId(providerId);
            d->m_cache->registerChangedEntry(entry);
            d->m_cache->writeRegistry();
        }
    }
#else
    Q_UNUSED(name);
    Q_UNUSED(providerId);
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
