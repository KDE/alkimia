/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alktestdefs.h"

#include "test.h"

#include <QSignalSpy>

#include <KIO/Job>

#include <iostream>

class TestKIOOffScreen : public QObject {
    Q_OBJECT
public:
    QString html;
public Q_SLOTS:
    void testFileCopy(const QUrl &url);
};

void TestKIOOffScreen::testFileCopy(const QUrl &url)
{
    QUrl dest = QUrl::fromLocalFile("/tmp/test.dat");

    KIO::Job *job = KIO::file_copy(url, dest, -1, KIO::HideProgressInfo | KIO::Overwrite);
    job->setUiDelegate(nullptr);

    QSignalSpy dataSpy(job, &KJob::result);
    QSignalSpy spy(job, &KJob::finished);
    spy.wait();
    QCOMPARE(spy.size(), 1);
    QCOMPARE(dataSpy.size(), 1);
    QCOMPARE(job->error(), 0);

    QFile f(dest.toLocalFile());
    if (f.exists()) {
        f.open(QAbstractSocket::ReadOnly);
        html = f.readAll();
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStringList args = app.arguments();
#ifdef USE_ERROR_URL
    QString s = TEST_DOWNLOAD_URL_ERROR;
#else
    QString s = TEST_DOWNLOAD_URL_PRICE;
#endif
    QUrl url = args.size() < 2 ? QUrl(s) : QUrl(args.at(1));

    TestKIOOffScreen test;
    test.testFileCopy(url);

    std::cout << test.html.toStdString();
    return 0;
}

#include "test-kio-cli.moc"
