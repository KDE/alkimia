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

class AlkOnlineQuotesProfileManager::Private
{
public:
  AlkOnlineQuotesProfileList m_profiles;
};

AlkOnlineQuotesProfileManager::AlkOnlineQuotesProfileManager()
  : d(new Private)
{
}

AlkOnlineQuotesProfileManager::~AlkOnlineQuotesProfileManager()
{
  delete d;
}

void AlkOnlineQuotesProfileManager::addProfile(AlkOnlineQuotesProfile *profile)
{
  if (!d->m_profiles.contains(profile)) {
    d->m_profiles.append(profile);
    profile->setManager(this);
  }
}

AlkOnlineQuotesProfileList AlkOnlineQuotesProfileManager::profiles()
{
  return d->m_profiles;
}
