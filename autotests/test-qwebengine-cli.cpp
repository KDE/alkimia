#include "alktestdefs.h"

#include "../3rdparty/qtwebengine/tests/auto/util/util.h"
#include "test.h"

#include <QApplication>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QTextEdit>
#include <QTimer>
#include <QPointer>

#include <iostream>

class TestQWebEngineOffScreen : public QObject {
    Q_OBJECT
public:
    QString html;
public Q_SLOTS:
    void toHtml(const QUrl &url);
};

void TestQWebEngineOffScreen::toHtml(const QUrl &url)
{
    QWebEngineProfile profile;
    QWebEnginePage page(&profile);
    QWebEngineView view;
    QSignalSpy loadFinishedSpy(&page, SIGNAL(loadFinished(bool)));
    view.setPage(&page);
    page.load(url);
    //view.show();
    //QTRY_COMPARE(view.isVisible(), true);
    QTRY_COMPARE_WITH_TIMEOUT(loadFinishedSpy.size() > 0, true, 20000);
    QCOMPARE(loadFinishedSpy.takeFirst().at(0).toBool(), true);

    CallbackSpy<QString> spy;
    page.toHtml(spy.ref());
    html = spy.waitForResult();
    QVERIFY(html.contains(TEST_LAUNCH_JAVASCRIPT_EXPECTED));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    QString s = TEST_LAUNCH_URL;
    s.append("a=EUR&dtype=javascript");
    QUrl url = args.size() < 2 ? QUrl(s) : QUrl(args.at(1));

    TestQWebEngineOffScreen test;
    test.toHtml(url);

    std::cout << test.html.toStdString();
    return 0;
}

#include "test-qwebengine-cli.moc"
