/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker  ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTEUPLOADDIALOG_H
#define ALKONLINEQUOTEUPLOADDIALOG_H

#include <QDialog>
#include <QUrl>

class AlkOnlineQuoteSource;

namespace Ui {
class AlkOnlineQuoteUploadDialog;
}

class AlkOnlineQuoteUploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlkOnlineQuoteUploadDialog(const AlkOnlineQuoteSource &source, bool autoGenerate = false, QWidget *parent = nullptr);
    ~AlkOnlineQuoteUploadDialog();

    static bool isSupported();

protected:
    bool compressFile(const QString &outFile, const QString &inFile);

private Q_SLOTS:
    void slotCreatePackage();
    void slotCopyToClipboard();
    void slotEnterStoreAndClose();

private:
    Ui::AlkOnlineQuoteUploadDialog *ui;
    const AlkOnlineQuoteSource &m_source;
    QUrl m_file;
    QUrl m_storePackageEditUrl;
};

#endif // ALKONLINEQUOTEUPLOADDIALOG_H
