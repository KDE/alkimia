/*
    SPDX-FileCopyrightText: 2019 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "applicationsettings.h"

#include <QDockWidget>
#include <QMainWindow>
#include <QSettings>

ApplicationSettings::ApplicationSettings(QMainWindow *parent, bool loadSettings)
  : m_parent(parent)
{
    if (loadSettings)
        readPositionSettings();
}

void ApplicationSettings::writePositionSettings()
{
    QSettings settings;

    settings.beginGroup("mainwindow");

    settings.setValue("geometry", m_parent->saveGeometry());
    settings.setValue("savestate", m_parent->saveState());
    settings.setValue("maximized", m_parent->isMaximized());
    if (!m_parent->isMaximized()) {
        settings.setValue("pos", m_parent->pos());
        settings.setValue("size", m_parent->size());
    }
    settings.endGroup();

    QList<QDockWidget*> dockWidgets = m_parent->findChildren<QDockWidget*>();
    foreach(QDockWidget *widget, dockWidgets)
    {
        settings.beginGroup(widget->objectName());
        settings.setValue("geometry", widget->saveGeometry());
        settings.setValue("maximized", widget->isMaximized());
        if (!widget->isMaximized()) {
            settings.setValue("pos", widget->pos());
            settings.setValue("size", widget->size());
        }
        settings.endGroup();
    }
}

void ApplicationSettings::readPositionSettings()
{
    QSettings settings;

    settings.beginGroup("mainwindow");

    m_parent->restoreGeometry(settings.value("geometry", m_parent->saveGeometry()).toByteArray());
    m_parent->restoreState(settings.value("savestate", m_parent->saveState()).toByteArray());
    m_parent->move(settings.value("pos", m_parent->pos()).toPoint());
    m_parent->resize(settings.value("size", m_parent->size()).toSize());
    if (settings.value("maximized", m_parent->isMaximized()).toBool())
        m_parent->showMaximized();

    settings.endGroup();

    QList<QDockWidget*> dockWidgets = m_parent->findChildren<QDockWidget*>();
    foreach(QDockWidget *widget, dockWidgets)
    {
        settings.beginGroup(widget->objectName());
        widget->restoreGeometry(settings.value("geometry", widget->saveGeometry()).toByteArray());
        widget->move(settings.value("pos", widget->pos()).toPoint());
        widget->resize(settings.value("size", widget->size()).toSize());
        if (settings.value("maximized", widget->isMaximized()).toBool())
            widget->showMaximized();
        settings.endGroup();
    }
}
