/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker  ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKUTILS_H
#define ALKUTILS_H

#include <alkimia/alk_export.h>

#include <QStringList>

class ALK_NO_EXPORT AlkUtils
{
public:
    static QStringList getDataFiles(const QString &path, const QStringList &extension);
    static QString locateDataFile(const QString &filePath);
};

#endif // ALKUTILS_H
