/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker  ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequoteuploaddialog.h"

#include "ui_alkonlinequoteuploaddialog.h"
#include "alkonlinequotesource.h"
#include "alknewstuffstore.h"

#include <QApplication>
#include <QClipboard>
#include <QDate>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>

static const char *packager = "/usr/bin/7za";


AlkOnlineQuoteUploadDialog::AlkOnlineQuoteUploadDialog(const AlkOnlineQuoteSource &source, bool autoGenerate, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlkOnlineQuoteUploadDialog)
    , m_source(source)
{
    ui->setupUi(this);
    ui->m_editSourceName->setText(source.name());
    ui->m_editVersion->setText(QDate::currentDate().toString(Qt::ISODate));
    ui->m_packageLabel->setVisible(false);
    ui->m_failedLabel->setVisible(false);
    QUrl packageUrl;
    QString id = m_source.profile()->GHNSId(source.name());
    if (!id.isEmpty()) {
        m_storePackageEditUrl = QUrl(AlkNewStuffStore::packageBaseUrl + id + AlkNewStuffStore::packageEditSuffix);
        packageUrl = QUrl(AlkNewStuffStore::packageBaseUrl + id);
        if (autoGenerate) {
            slotCreatePackage();
            slotCopyToClipboard();
            slotEnterStoreAndClose();
            QTimer::singleShot(0, this, SLOT(close()));
        }
    } else {
        m_storePackageEditUrl = QUrl(AlkNewStuffStore::newPackageUrl);
    }
    ui->m_storePackageEditUrl->setText(QString("<a href=\"%1\">%1</a>").arg(m_storePackageEditUrl.toString()));
    ui->m_storePackageUrl->setText(QString("<a href=\"%1\">%1</a>").arg(packageUrl.toString()));

    connect(ui->m_createButton, SIGNAL(clicked()), this, SLOT(slotCreatePackage()));
    connect(ui->m_enterStoreButtonAndClose, SIGNAL(clicked()), this, SLOT(slotEnterStoreAndClose()));
    connect(ui->m_copyToClipboardButton, SIGNAL(clicked()), this, SLOT(slotCopyToClipboard()));
    connect(ui->m_closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

AlkOnlineQuoteUploadDialog::~AlkOnlineQuoteUploadDialog()
{
    delete ui;
}

bool AlkOnlineQuoteUploadDialog::isSupported()
{
    QFileInfo fi(packager);
    return fi.exists();
}

bool AlkOnlineQuoteUploadDialog::compressFile(const QString &outFile, const QString &inFile)
{
    QProcess zip;
    QStringList args;
    args << "a"
         << outFile
         << inFile;

    zip.start(packager, args);
    if (!zip.waitForStarted())
        return false;

    if (!zip.waitForFinished())
        return false;

    return true;
}

void AlkOnlineQuoteUploadDialog::slotCreatePackage()
{
    QString tempPath = QDir::tempPath();
    QFileInfo inFile(m_source.ghnsWriteFileName());
    QFileInfo outFile(tempPath + "/" + inFile.baseName() + "-" + ui->m_editVersion->text() + ".zip");
    ui->m_copyToClipboardButton->setEnabled(true);
    m_file = outFile.absoluteFilePath();
    ui->m_fileLabel->setText(QString("<a href=\"%1\">%1</a>").arg(m_file.toString()));

    if (!compressFile(outFile.absoluteFilePath(), inFile.absoluteFilePath())) {
        ui->m_failedLabel->setVisible(true);
        return;
    }

    ui->m_packageLabel->setVisible(true);
    ui->m_createButton->setEnabled(false);
    ui->m_closeButton->setEnabled(true);
}

void AlkOnlineQuoteUploadDialog::slotCopyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_file.toString());
}

void AlkOnlineQuoteUploadDialog::slotEnterStoreAndClose()
{
    QDesktopServices::openUrl(m_storePackageEditUrl);
    accept();
}
