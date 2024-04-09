/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebview.h"
#include "alktestdefs.h"

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
#ifdef BUILD_WITH_WEBENGINE
        AlkWebView::setWebInspectorEnabled(true);
#endif
        view = new AlkWebView;
#ifdef BUILD_WITH_WEBKIT
        view->setWebInspectorEnabled(true);
#endif

        QGridLayout *layout = new QGridLayout;
        urlEdit = new QLineEdit;
        QPushButton *pushButton = new QPushButton("Load url");
        startedLabel = new QLabel;
        finishedLabel = new QLabel;
        redirectedLabel = new QLabel;
        connect(view, SIGNAL(loadRedirectedTo(QUrl)), this, SLOT(slotRedirectedTo(QUrl)));
        connect(view, SIGNAL(loadStarted()), this, SLOT(slotStarted()));
        connect(view, SIGNAL(loadFinished(bool)), this, SLOT(slotFinished(bool)));
        connect(pushButton, SIGNAL(pressed()), this, SLOT(slotPressed()));
        layout->addWidget(urlEdit, 0, 0, 1, 4);
        layout->addWidget(pushButton, 0, 5);
        layout->addWidget(view, 1, 0, 4, 4);
        layout->addWidget(new QLabel("Signals"), 1, 5, 1, 2);
        layout->addWidget(new QLabel("started:"), 2, 5);
        layout->addWidget(startedLabel, 2, 6);
        layout->addWidget(new QLabel("finished:"), 3, 5);
        layout->addWidget(finishedLabel, 3, 6);
        layout->addWidget(new QLabel("redirected:"), 4, 5);
        layout->addWidget(redirectedLabel, 4, 6);
        setLayout(layout);
        QString url = QLatin1String(TEST_DOWNLOAD_URL) + "&redirect=1";
        urlEdit->setText(url);
        view->load(url);
    }

public Q_SLOTS:
    void slotPressed()
    {
        QUrl url(urlEdit->text());
        startedLabel->setText("");
        finishedLabel->setText("");
        redirectedLabel->setText("");
        if (url.isValid())
            view->load(url);
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
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlkWebViewTestDialog dialog;
    dialog.show();
    app.exec();
}

#include "alkwebviewwidgettest.moc"
