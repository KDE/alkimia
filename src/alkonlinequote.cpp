/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkonlinequote.h"

#include "alkonlinequote_p.h"

AlkOnlineQuote::Errors::Errors()
{
}

AlkOnlineQuote::Errors::Errors(Type type)
{
    if (type != None)
        m_type.append(type);
    else
        m_type.clear();
}

AlkOnlineQuote::Errors& AlkOnlineQuote::Errors::operator|=(Type t)
{
    if (!m_type.contains(t)) {
        m_type.append(t);
    }
    return *this;
}

bool AlkOnlineQuote::Errors::operator &(Type t) const
{
    return m_type.contains(t);
}


AlkOnlineQuote::AlkOnlineQuote(AlkOnlineQuotesProfile *profile, QObject *_parent)
    : QObject(_parent)
    , d(new Private(this))
{
    if (profile)
        d->m_profile = profile;
    else {
        d->m_profile = new AlkOnlineQuotesProfile;
        d->m_ownProfile = true;
    }
}

AlkOnlineQuote::~AlkOnlineQuote()
{
    delete d;
}

AlkOnlineQuotesProfile *AlkOnlineQuote::profile()
{
    return d->m_profile;
}

void AlkOnlineQuote::setProfile(AlkOnlineQuotesProfile *profile)
{
    if (profile && d->m_ownProfile) {
        // switching from own profile to external
        delete d->m_profile;
        d->m_ownProfile = false;
        d->m_profile = profile;

    } else if (!profile && !d->m_ownProfile) {
        // switching from external to own profile
        d->m_profile = new AlkOnlineQuotesProfile;
        d->m_ownProfile = true;

    } else if (profile) {
        // exchange external profile
        d->m_profile = profile;
    }
}

void AlkOnlineQuote::setAcceptLanguage(const QString &language)
{
    d->m_acceptLanguage = language;
}

int AlkOnlineQuote::timeout() const
{
    return d->m_timeout;
}

void AlkOnlineQuote::setTimeout(int newTimeout)
{
    d->m_timeout = newTimeout;
}

void AlkOnlineQuote::setDateRange(const QDate &from, const QDate &to)
{
    d->m_startDate = from;
    d->m_endDate = to;
}

bool AlkOnlineQuote::launch(const QString &_symbol, const QString &_id, const QString &_source)
{
#ifdef ENABLE_FINANCEQUOTE
    if (AlkOnlineQuoteSource::isFinanceQuote(_source) ||
            d->m_profile->type() == AlkOnlineQuotesProfile::Type::Script) {
        return d->launchFinanceQuote(_symbol, _id, _source);
    } else
#endif
    if (_source.endsWith(QLatin1String(".css"))) {
        return d->launchWebKitCssSelector(_symbol, _id, _source);
    } else if (_source.endsWith(QLatin1String(".webkit"))) {
        return d->launchWebKitHtmlParser(_symbol, _id, _source);
    } else {
        return d->launchNative(_symbol, _id, _source);
    }
}

const AlkOnlineQuote::Errors &AlkOnlineQuote::errors()
{
    return d->m_errors;
}
