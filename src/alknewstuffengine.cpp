/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <knscore/engine.h>
    #include <knewstuff_version.h>
#else
    #include <knewstuff3/downloadmanager.h>
    #define KNEWSTUFF_VERSION 0
#endif

#include <QEventLoop>
#include <QtDebug>
#include <QPointer>
#include <QWidget>

class AlkNewStuffEngine::Private : public QObject
{
    Q_OBJECT
public:
    AlkNewStuffEngine *q;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QPointer<KNSCore::Engine> m_engine;
    bool m_providersLoaded{false};
    bool m_wantUpdates{false};
#else
    QPointer<KNS3::DownloadManager> m_engine;
#endif
    Private(AlkNewStuffEngine *parent);
    ~Private();

    bool init(const QString &configFile);
    void checkForUpdates();

public Q_SLOTS:
    void slotUpdatesAvailable(const KNS3::Entry::List &entries);
};

AlkNewStuffEngine::Private::Private(AlkNewStuffEngine *parent)
    : q(parent), m_engine(nullptr) {}

AlkNewStuffEngine::Private::~Private() { delete m_engine; }

bool AlkNewStuffEngine::Private::init(const QString &configFile)
{
    bool state = false;
#if KNEWSTUFF_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_engine = new KNSCore::Engine(this);
    state = m_engine->init(configFile);
    if (!state)
        return false;

    connect(m_engine, &KNSCore::Engine::signalProvidersLoaded, this, [this]()
    {
        qDebug() << Q_FUNC_INFO << "providers loaded";
        m_providersLoaded = true;
        if (m_wantUpdates)
        m_engine->checkForUpdates();
    });

    connect(m_engine, &KNSCore::Engine::signalUpdateableEntriesLoaded, this, [this](const KNSCore::EntryInternal::List &entries)
    {
        qDebug() << Q_FUNC_INFO << "updates loaded";
        AlkNewStuffEntryList updateEntries;
        for (const KNSCore::EntryInternal &entry : entries) {
        AlkNewStuffEntry e;
        e.category = entry.category();
        e.id = entry.uniqueId();
        e.installedFiles = entry.installedFiles();
        e.name = entry.name();
        e.providerId = entry.providerId();
        e.status =
            static_cast<AlkNewStuffEntry::Status>(entry.status());
        e.version = entry.version();
        updateEntries.append(e);
        qDebug() << Q_FUNC_INFO << e.name << toString(e.status);
        }
        Q_EMIT q->updatesAvailable(updateEntries);
    });
#else
    m_engine = new KNS3::DownloadManager(configFile, this);
    // no chance get the state
    state = true;

    connect(m_engine, SIGNAL(searchResult(KNS3::Entry::List)), this,
            SLOT(slotUpdatesAvailable(KNS3::Entry::List)));
#endif
    return state;
}

void AlkNewStuffEngine::Private::checkForUpdates()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (m_providersLoaded && !m_wantUpdates) {
        m_engine->checkForUpdates();
    } else
        m_wantUpdates = true;
#else
    m_engine->checkForUpdates();
#endif
}

void AlkNewStuffEngine::Private::slotUpdatesAvailable(const KNS3::Entry::List &entries)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_UNUSED(entries);
#else
    qDebug() << Q_FUNC_INFO << "updates loaded";
    AlkNewStuffEntryList updateEntries;
    for (const KNS3::Entry &entry : entries) {
        AlkNewStuffEntry e;
        e.category = entry.category();
        e.id = entry.id();
        e.installedFiles = entry.installedFiles();
        e.name = entry.name();
        e.providerId = entry.providerId();
        e.status = static_cast<AlkNewStuffEntry::Status>(entry.status());
        e.version = entry.version();
        updateEntries.append(e);

        qDebug() << Q_FUNC_INFO << e.name << toString(e.status);
    }

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
