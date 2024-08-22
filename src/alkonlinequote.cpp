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

bool AlkOnlineQuote::Errors::operator ==(Type t) const
{
    return m_type.size() == 1 && m_type.contains(t);
}

bool AlkOnlineQuote::Errors::operator !=(Type t) const
{
    return m_type.size() != 1 || !m_type.contains(t);
}

bool AlkOnlineQuote::Errors::isEmpty() const
{
    return m_type.size() == 0;
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

AlkOnlineQuote::LastPriceState AlkOnlineQuote::returnLastPriceState()
{
    return d->m_alwaysReturnLastPrice;
}

void AlkOnlineQuote::setReturnLastPriceState(LastPriceState state)
{
    d->m_alwaysReturnLastPrice = state;
}

void AlkOnlineQuote::setUseSingleQuoteSignal(bool state)
{
    d->m_useSingleQuoteSignal = state;
}

bool AlkOnlineQuote::useSingleQuoteSignal()
{
    return d->m_useSingleQuoteSignal;
}

bool AlkOnlineQuote::enableReverseLaunch()
{
    return d->m_reverseLaunchEnabled;
}

void AlkOnlineQuote::setEnableReverseLaunch(bool state)
{
    d->m_reverseLaunchEnabled = state;
}

const AlkOnlineQuoteSource &AlkOnlineQuote::source() const
{
    return d->m_source;
}

bool AlkOnlineQuote::launch(const QString &_symbol, const QString &_id, const QString &_source)
{
    return d->launch(_symbol, _id, _source);
}

const AlkOnlineQuote::Errors &AlkOnlineQuote::errors()
{
    return d->m_errors;
}

AlkOnlineQuote::Private &AlkOnlineQuote::d_ptr()
{
    return *d;
}
