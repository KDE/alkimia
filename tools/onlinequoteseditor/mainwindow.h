/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "applicationsettings.h"

class QUrl;

class MainWindow : public QMainWindow, public ApplicationSettings
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void slotUpdateAvailable(const QString &profile, const QString &name);
    void slotUrlChanged(const QUrl &url);
    void slotEditingFinished();
    void slotLanguageChanged(const QString &);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    class Private;
    Private *const d;
};

#endif // MAINWINDOW_H
