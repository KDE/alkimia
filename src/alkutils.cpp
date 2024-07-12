/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker  ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkutils.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QDir>
#include <QStandardPaths>

QStringList AlkUtils::getDataFiles(const QString &path, const QStringList &extensions)
{
    QStringList resources;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, path, QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString& dir, dirs) {
        const QStringList fileNames = QDir(dir).entryList(extensions);
        Q_FOREACH (const QString& file, fileNames) {
            resources.append(dir + '/' + file);
        }
    }
    return resources;
}

QString AlkUtils::locateDataFile(const QString &filePath)
{
    return QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("alkimia%1/%2").arg(TARGET_SUFFIX, filePath));
}

#else
#include <KGlobal>
#include <KStandardDirs>

QStringList AlkUtils::getDataFiles(const QString &path, const QStringList &extensions)
{
    const QString filename = QString("%1/%2").arg(path, extensions.at(0));
    const QStringList resources = KStandardDirs().findAllResources("data", filename);
    return resources;
}

QString AlkUtils::locateDataFile(const QString &filePath)
{
    return KGlobal::dirs()->findResource("appdata", filePath);
}
#endif
