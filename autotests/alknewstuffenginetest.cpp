/*
    SPDX-FileCopyrightText: 2024-2026 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include "alkdebug.h"
#include "test.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QSignalSpy>
#include <QTest>

class AlkNewStuffEngineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void entriesAvailable();
    void installDownloadableEntry();
    void updatesAvailable();

private:
    QString _configFile;
    QString _configFileV2;
};

void AlkNewStuffEngineTest::initTestCase()
{
    _configFile = QStringLiteral("%1/%2").arg(TEST_KNSRC_DIR, QStringLiteral("v1/test-store.knsrc"));

    QVERIFY(QFileInfo::exists(_configFile));

    _configFileV2 = QStringLiteral("%1/%2").arg(TEST_KNSRC_DIR, QStringLiteral("v2/test-store.knsrc"));

    QVERIFY(QFileInfo::exists(_configFileV2));
}

void AlkNewStuffEngineTest::cleanupTestCase()
{
    AlkNewStuffEngine engine;
    QSignalSpy spy(&engine, &AlkNewStuffEngine::entriesAvailable);

    qDebug() << "using knsrc file" << _configFile;
    engine.init(_configFile);

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 5000);

    const auto entries = qvariant_cast<AlkNewStuffEntryList>(spy.at(0).at(0));

    QCOMPARE(entries.size(), 1);
    auto entry = entries.first();
    QVERIFY(engine.uninstall(entry));
}

void AlkNewStuffEngineTest::entriesAvailable()
{
    AlkNewStuffEngine engine;
    QSignalSpy spy(&engine, &AlkNewStuffEngine::entriesAvailable);

    qDebug() << "using knsrc file" << _configFile;
    engine.init(_configFile);

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 5000);

    const auto entries = qvariant_cast<AlkNewStuffEntryList>(spy.at(0).at(0));

    QVERIFY(!entries.isEmpty());

    for (const auto &entry : entries) {
        alkDebug() << "entry loaded:" << entry.name << "status:" << entry.status;
    }
}

void AlkNewStuffEngineTest::installDownloadableEntry()
{
    AlkNewStuffEngine engine;
    QSignalSpy spy(&engine, &AlkNewStuffEngine::entriesAvailable);

    qDebug() << "using knsrc file" << _configFile;
    engine.init(_configFile);

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 5000);

    const auto entries = qvariant_cast<AlkNewStuffEntryList>(spy.at(0).at(0));

    // The test store intentionally contains exactly one entry.
    // Keep this test in sync with the fixture if additional entries are added.
    QCOMPARE(entries.size(), 1);

    auto entry = entries.first();

    alkDebug() << "entry" << entry.name << "has status" << entry.status;

    // The entry may still be installed from a previous test run.
    // Uninstall it first so that the installation below starts from a
    // known state.
    if (entry.status == AlkNewStuffEntry::Installed) {
        alkDebug() << "uninstalling" << entry.name;

        QVERIFY(engine.uninstall(entry));

        entry.status = AlkNewStuffEntry::Deleted;
    }

    QVERIFY(entry.status == AlkNewStuffEntry::Downloadable || entry.status == AlkNewStuffEntry::Deleted);

    QVERIFY(engine.install(entry));

    qDebug() << "entry" << entry << "installed";

    qDebug() << "provider id" << entry.providerId;
    engine.setProviderId(entry.name, TEST_DOWNLOAD_HOST_OCS "/ocs/v2/");
}

void AlkNewStuffEngineTest::updatesAvailable()
{
    AlkNewStuffEngine engine;
    QSignalSpy spy(&engine, &AlkNewStuffEngine::updatesAvailable);

    // switch to v2 with the same entry but updateable state
    qDebug() << "using knsrc file" << _configFileV2;
    engine.init(_configFileV2);

    engine.checkForUpdates();

    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, 5000);

    const auto entries = qvariant_cast<AlkNewStuffEntryList>(spy.at(0).at(0));

    QCOMPARE(entries.size(), 1);
    QVERIFY(entries.first().status == AlkNewStuffEntry::Updateable);

    for (const auto &entry : entries) {
        alkDebug() << "update found:" << entry.name;
    }
}

QTEST_MAIN(AlkNewStuffEngineTest)

#include "alknewstuffenginetest.moc"
