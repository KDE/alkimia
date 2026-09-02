/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdebug.h"
#include "alktestdefs.h"
#include "alkwebpage.h"
#include "alkwebview.h"

#include <QApplication>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class AlkWebViewTestDialog : public QDialog
{
    Q_OBJECT
public:
    AlkWebView *view;
    QLineEdit *urlEdit;
    QLabel *startedLabel;
    QLabel *finishedLabel;
    QLabel *redirectedLabel;

    AlkWebViewTestDialog()
    {
#if defined(ALKIMIA_WEBENGINE)
        AlkWebView::setWebInspectorEnabled(true);
#endif
        view = new AlkWebView;
        view->setWebPage(new AlkWebPage(view));

#if !defined(ALKIMIA_WEBENGINE)
        view->setWebInspectorEnabled(true);
#endif
        auto *mainLayout = new QVBoxLayout;

        //
        // Top: URL and load controls
        //
        auto *urlLayout = new QHBoxLayout;

        urlEdit = new QLineEdit;
        QPushButton *loadButton = new QPushButton("Load URL");
        QPushButton *loadTestRedirectButton = new QPushButton("Load URL with redirection");
        QPushButton *loadTestHtmlButton = new QPushButton("Load test HTML");

        urlLayout->addWidget(urlEdit);
        urlLayout->addWidget(loadButton);
        urlLayout->addWidget(loadTestRedirectButton);
        urlLayout->addWidget(loadTestHtmlButton);

        mainLayout->addLayout(urlLayout);

        //
        // Web view
        //
        mainLayout->addWidget(view, 1);

        //
        // Status information
        //
        auto *statusLayout = new QHBoxLayout;

        startedLabel = new QLabel;
        finishedLabel = new QLabel;
        redirectedLabel = new QLabel;

        statusLayout->addWidget(new QLabel("started:"));
        statusLayout->addWidget(startedLabel);

        statusLayout->addSpacing(10);

        statusLayout->addWidget(new QLabel("finished:"));
        statusLayout->addWidget(finishedLabel);

        statusLayout->addSpacing(10);

        statusLayout->addWidget(new QLabel("redirected:"));
        statusLayout->addWidget(redirectedLabel);

        statusLayout->addSpacing(10);

        statusLayout->addStretch();

        mainLayout->addLayout(statusLayout);

        setLayout(mainLayout);

        /*
         * AlkWebView exposes the same signals for all supported backends.
         *
         * Use the old SIGNAL/SLOT syntax here because loadStarted/loadFinished
         * are not available with the same interface on every backend.
         */
        connect(view, SIGNAL(loadRedirectedTo(QUrl)),
                this, SLOT(slotRedirectedTo(QUrl)));

        connect(view, SIGNAL(loadStarted()),
                this, SLOT(slotStarted()));

        connect(view, SIGNAL(loadFinished(bool)),
                this, SLOT(slotFinished(bool)));

        connect(loadButton, SIGNAL(pressed()),
                this, SLOT(slotPressed()));

        connect(loadTestRedirectButton, SIGNAL(pressed()),
                this, SLOT(slotLoadRedirectTest()));

        connect(loadTestHtmlButton, SIGNAL(pressed()),
                this, SLOT(slotLoadTestHtml()));

        QString url = QLatin1String(TEST_DOWNLOAD_URL_CURRENCY);
        urlEdit->setText(url);

        view->load(url);
    }

public Q_SLOTS:
    void slotPressed()
    {
        QUrl url(urlEdit->text());

        resetStatus();

        if (url.isValid())
            view->load(url);
    }

    void slotLoadRedirectTest()
    {
        resetStatus();
        const QString url = QLatin1String(TEST_DOWNLOAD_URL_CURRENCY) + "&redirect=1";
        urlEdit->setText(url);
        view->load(url);
    }

    void slotLoadTestHtml()
    {
        resetStatus();

        /*
         * Keep this page self-contained.  This makes testing linkClicked()
         * and openLinks independent of network availability.
         *
         * The links deliberately point to external URLs.  When openLinks()
         * is disabled, clicking one must still emit linkClicked(), but must
         * not navigate away from this page.
         */
        const QUrl baseUrl = QUrl::fromLocalFile(CMAKE_CURRENT_SOURCE_DIR "/");
        QString html = QStringLiteral(
                           "<html>"
                           "<head>"
                           "<title>AlkWebView link test</title>"
                           "</head>"
                           "<body>"
                           "<h1>AlkWebView link test</h1>"
                           "<p>Click one of the following links:</p>"
                           "<ul>"
                           "<li><a href=\"https://www.example.com/\">Example.com</a></li>"
                           "<li><a href=\"https://www.kde.org/\">KDE</a></li>"
                           "<li><a href=\"testfile.txt\">a relative test path</a></li>"
                           "<li><a href=\"%1testfile.txt\">an absolute test path</a></li>"
                           "</ul>"
                           "<p>"
                           "With <b>Open links</b> enabled, the browser navigates to the "
                           "clicked URL."
                           "</p>"
                           "<p>"
                           "With <b>Open links</b> disabled, the browser remains on this "
                           "page while the <code>linkClicked</code> signal is still emitted."
                           "</p>"
                           "</body>"
                           "</html>")
                           .arg(baseUrl.toString());
        view->setHtml(html, baseUrl);
    }

    void slotStarted()
    {
        startedLabel->setText("ok");
    }

    void slotRedirectedTo(const QUrl &url)
    {
        if (url.isValid()) {
            redirectedLabel->setText("ok");
            urlEdit->setText(url.toString());
        }
    }

    void slotFinished(bool ok)
    {
        finishedLabel->setText(ok ? "ok" : "failed");
    }

private:
    void resetStatus()
    {
        startedLabel->setText("");
        finishedLabel->setText("");
        redirectedLabel->setText("");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlkWebViewTestDialog dialog;
    dialog.resize(1000, 600);
    dialog.show();

    return app.exec();
}

#include "alkwebviewwidgettest.moc"
