/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKNEWSTUFFWIDGET_H
#define ALKNEWSTUFFWIDGET_H

#include <alkimia/alk_export.h>

#include <QList>
#include <QStringList>
#include <QObject>

/**
 * Wrapper for new stuff widgets
 *
 * @author Ralf Habacker
 */
class ALK_NO_EXPORT AlkNewStuffWidget : public QObject
{
    Q_OBJECT
public:
    explicit AlkNewStuffWidget(QObject *parent = nullptr);
    bool init(const QString &configFile);
    bool showInstallDialog(QWidget *parent = nullptr);

private:
    class Private;
    Private *const d;
};

#endif // ALKNEWSTUFFWIDGET_H
