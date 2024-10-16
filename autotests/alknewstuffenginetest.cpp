/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffengine.h"

#include "alkdebug.h"

#include <QApplication>
#include <QObject>
#include <QTimer>

class TestReceiver : public QObject
{
    Q_OBJECT
public:
    QEventLoop &_loop;

    TestReceiver(QEventLoop &loop)
        : _loop(loop)
    {
        QTimer::singleShot(10000, this, SLOT(quitWithError()));
        connect(this, SIGNAL(finished()), this, SLOT(quit()));
    }

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void updatesAvailable(const AlkNewStuffEntryList &entries)
    {
        for (const AlkNewStuffEntry &entry : entries) {
            alkDebug() << entry.name;
        }
        Q_EMIT finished();
    }

    void quitWithError()
    {
        alkDebug() << "Timeout received - exit with error";
        _loop.exit(1);
    }

    void quit()
    {
        _loop.exit(0);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString configFile = QString("%1/%2").arg(KNSRC_DIR, "alkimia-quotes.knsrc");

    QEventLoop loop;
    TestReceiver receiver(loop);
    AlkNewStuffEngine engine(&receiver);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QObject::connect(&engine, &AlkNewStuffEngine::updatesAvailable, &receiver, &TestReceiver::updatesAvailable);
#else
    QObject::connect(&engine, SIGNAL(updatesAvailable(const AlkNewStuffEntryList &)), &receiver, SLOT(updatesAvailable(const AlkNewStuffEntryList &)));
#endif
    engine.init(configFile);
    engine.checkForUpdates();

    return loop.exec();
}

#include "alknewstuffenginetest.moc"
