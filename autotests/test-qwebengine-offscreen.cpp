// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

// from https://github.com/qt/qtwebengine/blob/ac447e8dc2f0b0c711ca17211646784dc9c5395f/tests/auto/widgets/offscreen/tst_offscreen.cpp#L10

#include "alktestdefs.h"

#include "../3rdparty/qtwebengine/tests/auto/util/util.h"
#include "test.h"

#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QTimer>

class TestQWebEngineOffScreen : public QObject {
    Q_OBJECT
public:
    TestQWebEngineOffScreen(){}

private Q_SLOTS:
    void toHtml();
};

void TestQWebEngineOffScreen::toHtml()
{
    QWebEngineProfile profile;
    QWebEnginePage page(&profile);
    QWebEngineView view;
    QSignalSpy loadFinishedSpy(&page, SIGNAL(loadFinished(bool)));
    view.setPage(&page);
    QString s = TEST_LAUNCH_URL;
    s.append("a=EUR&dtype=javascript");
    page.load(QUrl(s));
    // view.show();
    // QTRY_COMPARE(view.isVisible(), true);
    QTRY_COMPARE_WITH_TIMEOUT(loadFinishedSpy.size() > 0, true, 20000);
    QCOMPARE(loadFinishedSpy.takeFirst().at(0).toBool(), true);

    CallbackSpy<QString> spy;
    page.toHtml(spy.ref());
    QString html = spy.waitForResult();
    qDebug() << html;
    QVERIFY(html.contains(TEST_LAUNCH_JAVASCRIPT_EXPECTED));
}

QTEST_MAIN(TestQWebEngineOffScreen)

#include "test-qwebengine-offscreen.moc"
