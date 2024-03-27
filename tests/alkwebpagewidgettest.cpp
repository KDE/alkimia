/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkwebpage.h"
#include "alktestdefs.h"

#include <QApplication>
#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

class AlkWebPageTestDialog : public QDialog
{
    Q_OBJECT
public:
    AlkWebPage *page;
    QLineEdit *urlEdit;
    AlkWebPageTestDialog()
    {
        page = new AlkWebPage;

        QGridLayout *layout = new QGridLayout;
        urlEdit = new QLineEdit;
        QPushButton *pushButton = new QPushButton("Load url");
        connect(urlEdit, SIGNAL(editingFinished()), this, SLOT(slotPressed()));
        connect(pushButton, SIGNAL(pressed()), this, SLOT(slotPressed()));
        layout->addWidget(urlEdit, 0, 0);
        layout->addWidget(pushButton, 0, 1);
        layout->addWidget(page->widget(), 1, 0, 4, 2);
        setLayout(layout);
        QString url = QLatin1String(TEST_DOWNLOAD_URL) + "&redirect=1";
        urlEdit->setText(url);
        page->load(url, QLatin1String(""));
    }
public Q_SLOTS:
    void slotPressed()
    {
        QUrl url(urlEdit->text());
        if (url.isValid())
            page->load(url, "");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlkWebPageTestDialog dialog;
    dialog.show();
    app.exec();
}

#include "alkwebpagewidgettest.moc"
