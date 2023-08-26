/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesprofile.h"
#include "alkwebpage.h"

#include <QPointer>

#if QT_VERSION > QT_VERSION_CHECK(5,0,0)
#include <QLocale>
#define initLocale() QLocale()
#else
#include <KGlobal>
#define initLocale() KGlobal::locale()
#endif

class AlkOnlineQuotesProfileManager::Private
{
public:
    AlkOnlineQuotesProfileList m_profiles;
    QPointer<AlkWebPage> m_page;
    bool m_withPage;
    Private()
      : m_withPage(false)
    {
    }

    ~Private()
    {
        m_page.data()->deleteLater();
    }
};

AlkOnlineQuotesProfileManager::AlkOnlineQuotesProfileManager()
  : d(new Private)
{
}

AlkOnlineQuotesProfileManager::~AlkOnlineQuotesProfileManager()
{
    delete d;
}

bool AlkOnlineQuotesProfileManager::webPageEnabled()
{
    return d->m_withPage;
}

void AlkOnlineQuotesProfileManager::setWebPageEnabled(bool enable)
{
    d->m_withPage = enable;
}

void AlkOnlineQuotesProfileManager::addProfile(AlkOnlineQuotesProfile *profile)
{
    if (!d->m_profiles.contains(profile)) {
        d->m_profiles.append(profile);
        profile->setManager(this);
        connect(profile, SIGNAL(updateAvailable(const QString &, const QString &)), this, SIGNAL(updateAvailable(const QString &, const QString &)));
    }
}

AlkOnlineQuotesProfileList AlkOnlineQuotesProfileManager::profiles()
{
    return d->m_profiles;
}

AlkOnlineQuotesProfile *AlkOnlineQuotesProfileManager::profile(const QString &name)
{
    for (AlkOnlineQuotesProfile *profile : profiles()) {
        if (name == profile->name()) {
            return profile;
        }
    }
    return nullptr;
}

QStringList AlkOnlineQuotesProfileManager::profileNames()
{
    QStringList profiles;
    for (AlkOnlineQuotesProfile *profile : d->m_profiles) {
        profiles.append(profile->name());
    }
    return profiles;
}

AlkWebPage *AlkOnlineQuotesProfileManager::webPage()
{
    if (!d->m_page) {
    // make sure that translations are installed on windows
        initLocale();
        d->m_page = new AlkWebPage;
    }
    return d->m_page;
}

AlkOnlineQuotesProfileManager &AlkOnlineQuotesProfileManager::instance()
{
    static AlkOnlineQuotesProfileManager manager;
    return manager;
}
