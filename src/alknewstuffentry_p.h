/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKNEWSTUFFENTRY_P_H
#define ALKNEWSTUFFENTRY_P_H

#include "alkdebug.h"
#include "alknewstuffentry.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <knscore/entryinternal.h>
#else
    #include <knewstuff3/entry.h>
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
ALK_EXPORT QDebug operator<<(QDebug out, const KNSCore::EntryInternal &entry);
ALK_EXPORT QDebug operator<<(QDebug out, const KNSCore::EntryInternal::List &entries);
ALK_EXPORT AlkNewStuffEntry toAlkEntry(const KNSCore::EntryInternal &entry);
ALK_EXPORT void toAlkEntryList(AlkNewStuffEntryList &result, const KNSCore::EntryInternal::List &entries);
#else
ALK_EXPORT QDebug operator<<(QDebug out, const KNS3::Entry &entry);
ALK_EXPORT QDebug operator<<(QDebug out, const KNS3::Entry::List &entries);
ALK_EXPORT AlkNewStuffEntry toAlkEntry(const KNS3::Entry &entry);
ALK_EXPORT void toAlkEntryList(AlkNewStuffEntryList &result, const KNS3::Entry::List &entries);
#endif

#endif // ALKNEWSTUFFENGINE_P_H
