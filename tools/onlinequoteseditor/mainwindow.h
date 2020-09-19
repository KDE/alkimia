/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

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
