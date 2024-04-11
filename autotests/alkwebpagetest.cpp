/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebpagetest.h"

#include "alkwebpage.h"
#include "alktestdefs.h"

#include "test.h"

#include <QSignalSpy>

void AlkWebPageTest::testLoad()
{
    AlkWebPage page;

    QSignalSpy spyLoadStarted(&page, SIGNAL(loadStarted()));
    QSignalSpy spyLoadFinished(&page, SIGNAL(loadFinished(bool)));

    page.load(QUrl(TEST_LAUNCH_URL), QString());

    // test signals
    QTRY_COMPARE_WITH_TIMEOUT(spyLoadFinished.size() > 0, true, 20000);
    QCOMPARE(spyLoadFinished.takeFirst().at(0).toBool(), true);
    QVERIFY(spyLoadStarted.count() >= 1);

    // test content
    QVERIFY(page.toHtml().contains(QLatin1String("</body></html>")));
}

void AlkWebPageTest::testRedirected()
{
    AlkWebPage page;

    QSignalSpy spyLoadStarted(&page, SIGNAL(loadStarted()));
    QSignalSpy spyLoadFinished(&page, SIGNAL(loadFinished(bool)));
    QSignalSpy spyLoadRedirected(&page, SIGNAL(loadRedirectedTo(QUrl)));

    QString url(TEST_LAUNCH_URL);
    page.load(QUrl(url + "&redirect=1"), QString());

    // test signals
    QTRY_COMPARE_WITH_TIMEOUT(spyLoadFinished.size() > 0, true, 20000);
    QCOMPARE(spyLoadFinished.takeFirst().at(0).toBool(), true);
    QVERIFY(spyLoadStarted.count() >= 1);

#if defined(BUILD_WITH_WEBENGINE) && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QCOMPARE(spyLoadRedirected.count(), 1);
    QCOMPARE(spyLoadRedirected.takeFirst().at(0).toUrl(), QUrl(url));
#else
    QWARN("This engine does not emit signal loadRedirectedTo()");
    QWARN("This engine does not return a redirected url");
#endif
    // test content
    QVERIFY(page.toHtml().contains(QLatin1String("</body></html>")));
}

QTEST_MAIN(AlkWebPageTest)
