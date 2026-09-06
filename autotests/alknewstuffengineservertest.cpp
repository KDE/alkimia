/*
    SPDX-FileCopyrightText: 2026 Thomas Baumgart <tbaumgart@kde.org> (with support of AI)

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

/*
 * Fully local, controllable test for the KHotNewStuff update flow of
 * AlkNewStuffEngine.
 *
 * Instead of talking to the live KDE store (as alknewstuffenginetest.cpp does),
 * this test:
 *   1. starts a small Python OCS server (autotests/knstest/ocs_server.py) on an
 *      ephemeral local port,
 *   2. renders a .knsrc and a seeded .knsregistry from templates, substituting
 *      the server URL and an isolated data directory,
 *   3. points HOME / XDG_DATA_HOME at a temporary directory so the real user
 *      registry is never touched,
 *   4. runs checkForUpdates() and verifies that exactly one entry is reported
 *      as an available update.
 *
 * Determinism: the server advertises the content at version "2.0" while the
 * seeded registry records the same entry (same id + providerid) at version
 * "1.0", so KNSCore must classify it as updateable.
 */

#include "alknewstuffengine.h"
#include "alknewstuffentry.h"

#include "test.h"

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTextStream>
#include <QUrl>

class AlkNewStuffEngineServerTest : public QObject
{
    Q_OBJECT

    QTemporaryDir m_tmp;
    QProcess m_server;
    QString m_baseUrl;
    QString m_knsrcPath;

private:
    // Read one line of the form "LISTENING <host> <port>" from the server and
    // build its base URL. Returns false on timeout.
    bool waitForServer()
    {
        QByteArray line;
        QElapsedTimer timer;
        timer.start();
        while (timer.elapsed() < 10000) {
            if (m_server.state() == QProcess::NotRunning && m_server.bytesAvailable() == 0) {
                return false;
            }
            if (m_server.waitForReadyRead(500)) {
                line += m_server.readLine();
                if (line.contains('\n')) {
                    break;
                }
            }
        }
        const QList<QByteArray> parts = line.trimmed().split(' ');
        if (parts.size() != 3 || parts.at(0) != "LISTENING") {
            return false;
        }
        m_baseUrl = QStringLiteral("http://%1:%2").arg(QString::fromUtf8(parts.at(1)), QString::fromUtf8(parts.at(2)));
        return true;
    }

    // Copy a template file, replacing @KEY@ placeholders.
    bool render(const QString &inFile, const QString &outFile, const QMap<QString, QString> &vars)
    {
        QFile in(inFile);
        if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return false;
        }
        QString content = QString::fromUtf8(in.readAll());
        in.close();
        for (auto it = vars.constBegin(); it != vars.constEnd(); ++it) {
            content.replace(it.key(), it.value());
        }
        QFile out(outFile);
        if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }
        out.write(content.toUtf8());
        out.close();
        return true;
    }

private Q_SLOTS:
    void initTestCase()
    {
        qRegisterMetaType<AlkNewStuffEntry>();
        qRegisterMetaType<AlkNewStuffEntryList>();

        QVERIFY(m_tmp.isValid());

        // Isolate KNS data/registry into the temp dir so the real user
        // environment is never modified.
        const QString dataHome = m_tmp.path() + QStringLiteral("/data");
        QVERIFY(QDir().mkpath(dataHome));
        qputenv("XDG_DATA_HOME", dataHome.toUtf8());
        qputenv("HOME", m_tmp.path().toUtf8());
        // Reset QStandardPaths caches so the new env vars take effect.
        QStandardPaths::setTestModeEnabled(false);

        // Locate the python server and templates (KNSTEST_DIR is set by CMake).
        const QString testDir = QStringLiteral(KNSTEST_DIR);
        const QString serverScript = testDir + QStringLiteral("/ocs_server.py");
        QVERIFY2(QFile::exists(serverScript), qPrintable(serverScript));

        // Start the OCS server on an ephemeral port.
        m_server.setProcessChannelMode(QProcess::SeparateChannels);
        m_server.start(QStringLiteral(PYTHON_EXECUTABLE),
                       {serverScript, QStringLiteral("--host"), QStringLiteral("127.0.0.1"),
                        QStringLiteral("--port"), QStringLiteral("0"),
                        QStringLiteral("--version"), QStringLiteral("2.0")});
        QVERIFY2(m_server.waitForStarted(5000), "python server did not start");
        QVERIFY2(waitForServer(), "python server did not report LISTENING <host> <port>");

        // KNS derives the registry file name from the knsrc base name and looks
        // it up under <XDG_DATA_HOME>/knewstuff3/<name>.knsregistry.
        const QString knsName = QStringLiteral("alkimia-quotes-test");
        const QString knsDir = dataHome + QStringLiteral("/knewstuff3");
        QVERIFY(QDir().mkpath(knsDir));

        const QString installedDir = dataHome + QStringLiteral("/alkimia-test/quotes");
        QVERIFY(QDir().mkpath(installedDir));
        const QString installedFile = installedDir + QStringLiteral("/Test.txt");
        {
            QFile f(installedFile);
            QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Text));
            f.write("local test quote source\n");
            f.close();
        }

        QMap<QString, QString> vars;
        vars.insert(QStringLiteral("@OCS_BASE_URL@"), m_baseUrl);
        vars.insert(QStringLiteral("@OCS_PROVIDER_ID@"), QUrl(m_baseUrl).host());
        vars.insert(QStringLiteral("@INSTALLED_FILE@"), installedFile);

        // Render knsrc into the temp dir.
        m_knsrcPath = m_tmp.path() + QStringLiteral("/") + knsName + QStringLiteral(".knsrc");
        QVERIFY(render(testDir + QStringLiteral("/alkimia-quotes-test.knsrc.in"), m_knsrcPath, vars));

        // Render the seeded (older-version) registry.
        QVERIFY(render(testDir + QStringLiteral("/alkimia-quotes-test.knsregistry.in"),
                       knsDir + QStringLiteral("/") + knsName + QStringLiteral(".knsregistry"), vars));
    }

    void cleanupTestCase()
    {
        if (m_server.state() != QProcess::NotRunning) {
            m_server.terminate();
            if (!m_server.waitForFinished(3000)) {
                m_server.kill();
            }
        }
    }

    // The engine must load the seeded installed entry from the registry.
    void testInstalledEntries()
    {
        AlkNewStuffEngine engine;
        QVERIFY2(engine.init(m_knsrcPath), "engine init failed");

        const AlkNewStuffEntryList installed = engine.installedEntries();
        QCOMPARE(installed.size(), 1);
        QCOMPARE(installed.at(0).id, QStringLiteral("1000"));
        QCOMPARE(installed.at(0).version, QStringLiteral("1.0"));
    }

    // checkForUpdates() must report exactly one updateable entry, because the
    // server advertises version 2.0 while the registry holds 1.0.
    void testCheckForUpdates()
    {
        AlkNewStuffEngine engine;
        QSignalSpy spy(&engine, &AlkNewStuffEngine::updatesAvailable);
        QVERIFY(spy.isValid());

        QVERIFY2(engine.init(m_knsrcPath), "engine init failed");
        engine.checkForUpdates();

        // Providers load + OCS round-trip happen asynchronously.
        QVERIFY2(spy.wait(15000), "updatesAvailable was not emitted");

        const QList<QVariant> args = spy.takeFirst();
        const AlkNewStuffEntryList updates = args.at(0).value<AlkNewStuffEntryList>();

        QCOMPARE(updates.size(), 1);
        QCOMPARE(updates.at(0).id, QStringLiteral("1000"));
        // The engine reports the installed entry flagged as updateable; the
        // AlkNewStuffEntry carries the installed version (1.0), while the newer
        // server version (2.0) is what triggered the update classification.
        QCOMPARE(updates.at(0).status, AlkNewStuffEntry::Updateable);
        QCOMPARE(updates.at(0).version, QStringLiteral("1.0"));
    }
};

QTEST_GUILESS_MAIN(AlkNewStuffEngineServerTest)

#include "alknewstuffengineservertest.moc"
