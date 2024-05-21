/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef CACHE_H
#define CACHE_H

#include "alkdebug.h"

#include <knewstuff3/core/entryinternal.h>

#include <kstandarddirs.h>

#include <QFile>
#include <QFileInfo>

class Cache
{
public:
    QString registryFile;
    QSet<KNS3::EntryInternal> cache;

    Cache(const QString &appName)
    {
        registryFile = KStandardDirs::locateLocal("data", "knewstuff3/" + appName + ".knsregistry");
        alkDebug() << "Using registry file: " << registryFile;
    }

    void readRegistry()
    {
        QFile f(registryFile);
        if (!f.open(QIODevice::ReadOnly)) {
            qWarning() << "The file " << registryFile << " could not be opened.";
            return;
        }

        QDomDocument doc;
        if (!doc.setContent(&f)) {
            qWarning() << "The file could not be parsed.";
            return;
        }

        QDomElement root = doc.documentElement();
        if (root.tagName() != "hotnewstuffregistry") {
            qWarning() << "The file doesn't seem to be of interest.";
            return;
        }

        QDomElement stuff = root.firstChildElement("stuff");
        cache.clear();
        while (!stuff.isNull()) {
            KNS3::EntryInternal e;
            e.setEntryXML(stuff);
            e.setSource(KNS3::EntryInternal::Cache);
            // remove double path separators
            // see https://bugs.kde.org/show_bug.cgi?id=484016
            QStringList files = e.installedFiles();
            for (QString &f : files) {
                f.replace("//", "/");
            }
            e.setInstalledFiles(files);
            cache.insert(e);
            stuff = stuff.nextSiblingElement("stuff");
        }

        alkDebug() << "Cache read... entries: " << cache.size();
    }

    KNS3::Entry::List registry()
    {
        KNS3::Entry::List result;
        for (const KNS3::EntryInternal &e : cache) {
            result.append(e.toEntry());
        }
        return result;
    }
};
#endif
