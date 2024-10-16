/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffentry.h"

#include "alknewstuffentry_p.h"

QDebug operator<<(QDebug out, const AlkNewStuffEntry &entry)
{
    out << entry.name << entry.status;
    return out;
}

QDebug operator<<(QDebug out, const AlkNewStuffEntryList &entries)
{
    out << "AlkNewStuffEntryList(";
    for (const auto &e : entries) {
        out << e;
    }
    out << ")";

    return out;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QDebug operator<<(QDebug out, const KNSCore::Entry::List &entries)
{
    out << "KNSCore::Entry::List";
    for (auto &e : entries) {
        out << e;
    }

    return out;
}

AlkNewStuffEntry toAlkEntry(const KNSCore::Entry &entry)
{
    AlkNewStuffEntry e;
    e.category = entry.category();
    e.id = entry.uniqueId();
    e.installedFiles = entry.installedFiles();
    e.name = entry.name();
    e.providerId = entry.providerId();
    e.status = static_cast<AlkNewStuffEntry::Status>(entry.status());
    e.version = entry.version();
    return e;
}

void toAlkEntryList(AlkNewStuffEntryList &result, const KNSCore::Entry::List &entries)
{
    for (const KNSCore::Entry &entry : entries) {
        result.append(toAlkEntry(entry));
    }
}
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
QDebug operator<<(QDebug out, const KNSCore::EntryInternal &entry)
{
    out << entry.name() << entry.status();
    return out;
}

QDebug operator<<(QDebug out, const KNSCore::EntryInternal::List &entries)
{
    out << "KNSCore::EntryInternal::List";
    for (auto &e : entries) {
        out << e;
    }

    return out;
}

AlkNewStuffEntry toAlkEntry(const KNSCore::EntryInternal &entry)
{
    AlkNewStuffEntry e;
    e.category = entry.category();
    e.id = entry.uniqueId();
    e.installedFiles = entry.installedFiles();
    e.name = entry.name();
    e.providerId = entry.providerId();
    e.status = static_cast<AlkNewStuffEntry::Status>(entry.status());
    e.version = entry.version();
    return e;
}

void toAlkEntryList(AlkNewStuffEntryList &result, const KNSCore::EntryInternal::List &entries)
{
    for (const KNSCore::EntryInternal &entry : entries) {
        result.append(toAlkEntry(entry));
    }
}
#else
QDebug operator<<(QDebug out, const KNS3::Entry &entry)
{
    out << entry.name() << entry.status();
    return out;
}

QDebug operator<<(QDebug out, const KNS3::Entry::List &entries)
{
    out << "KNS3::Entry::List(";
    for (auto &e : entries) {
        out << e;
    }
    out << ")";

    return out;
}

AlkNewStuffEntry toAlkEntry(const KNS3::Entry &entry)
{
    AlkNewStuffEntry e;
    e.category = entry.category();
    e.id = entry.id();
    e.installedFiles = entry.installedFiles();
    e.name = entry.name();
    e.providerId = entry.providerId();
    e.status = static_cast<AlkNewStuffEntry::Status>(entry.status());
    e.version = entry.version();
    return e;
}

void toAlkEntryList(AlkNewStuffEntryList &result, const KNS3::Entry::List &entries)
{
    for (const KNS3::Entry &entry : entries) {
        result.append(toAlkEntry(entry));
    }
}
#endif
