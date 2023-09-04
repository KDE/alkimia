/*
    SPDX-FileCopyrightText: 2019 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

class QMainWindow;

class ApplicationSettings
{
protected:
    explicit ApplicationSettings(QMainWindow *parent, bool loadSettings = true);
    void writePositionSettings();
    void readPositionSettings();
    QMainWindow *m_parent;
};

#endif // APPLICATIONSETTINGS_H
