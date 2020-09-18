/***************************************************************************
 *   Copyright 2018  Ralf Habacker <ralf.habacker@freenet.de>              *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
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

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesprofile.h"
#include "alkwebpage.h"

#include <QPointer>

#include <KGlobal>

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
    foreach (AlkOnlineQuotesProfile *profile, profiles()) {
        if (name == profile->name()) {
            return profile;
        }
    }
    return nullptr;
}

QStringList AlkOnlineQuotesProfileManager::profileNames()
{
    QStringList profiles;
    foreach(AlkOnlineQuotesProfile *profile, AlkOnlineQuotesProfileManager::instance().profiles()) {
        profiles.append(profile->name());
    }
    return profiles;
}

AlkWebPage *AlkOnlineQuotesProfileManager::webPage()
{
    if (!d->m_page) {
    // make sure that translations are installed on windows
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        KGlobal::locale();
#else
        KLocale::global();
#endif
        d->m_page = new AlkWebPage;
    }
    return d->m_page;
}

AlkOnlineQuotesProfileManager &AlkOnlineQuotesProfileManager::instance()
{
    static AlkOnlineQuotesProfileManager manager;
    return manager;
}
