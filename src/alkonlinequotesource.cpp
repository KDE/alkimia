/*
 * SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker@freenet.de
 * SPDX-FileCopyrightText: 2023 Thomas Baumgart <tbaumgart@kde.org>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of libalkimia.
 */

#include "alkonlinequotesource.h"

#include "alkonlinequotesprofile.h"

#include "alkonlinequotesource_p.h"

/**
 * Key to identifying "Finance::Quote" sources
 */
static const char *fqName = "Finance::Quote";

AlkOnlineQuoteSource::AlkOnlineQuoteSource()
    : d(new Private)
{
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const AlkOnlineQuoteSource &other)
    : d(new Private(other.d))
{
}

AlkOnlineQuoteSource &AlkOnlineQuoteSource::operator=(AlkOnlineQuoteSource other)
{
    swap(*this, other);
    return *this;
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const QString& name,
                                           const QString& url,
                                           const QString& idRegex,
                                           const IdSelector idBy,
                                           const QString& priceRegex,
                                           const QString& dateRegex,
                                           const QString& dateFormat,
                                           DataFormat dataFormat,
                                           DecimalSeparator priceDecimalSeparator,
                                           DownloadType downloadType)
    : d(new Private)
{
    d->m_name = name;
    d->m_url = url;
    d->m_idRegex = idRegex;
    d->m_idSelector = idBy;
    d->m_priceDecimalSeparator = priceDecimalSeparator;
    d->m_priceRegex = priceRegex;
    d->m_dataFormat = dataFormat;
    d->m_dateRegex = dateRegex;
    d->m_dateFormat = dateFormat;
    d->m_downloadType = downloadType;
    d->m_isGHNSSource = false;
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const QString &name, AlkOnlineQuotesProfile *profile)
    : d(new Private)
{
    if ((profile != nullptr) && (profile->type() == AlkOnlineQuotesProfile::Type::None) && (profile->defaultQuoteSources().contains(name))) {
        *this = profile->defaultQuoteSources()[name];
    } else {
        d->m_profile = profile;
        d->m_name = name;
        read();
    }
}

AlkOnlineQuoteSource AlkOnlineQuoteSource::defaultCurrencyQuoteSource(const QString& name)
{
    return AlkOnlineQuoteSource(name,
                                "https://fx-rate.net/%1/%2",
                                QString(), // idregexp
                                AlkOnlineQuoteSource::Symbol,
                                "Today\\s+=\\s+([^<]+)",
                                ",\\s*(\\d+\\s*[a-zA-Z]{3}\\s*\\d{4})",
                                "%d %m %y",
                                HTML
    );
}

AlkOnlineQuoteSource::~AlkOnlineQuoteSource()
{
    delete d;
}

bool AlkOnlineQuoteSource::isEmpty()
{
    return !isValid()  && !d->m_url.isEmpty();
}

bool AlkOnlineQuoteSource::isValid()
{
    return !d->m_name.isEmpty();
}

QString AlkOnlineQuoteSource::name() const
{
    return d->m_name;
}

QString AlkOnlineQuoteSource::url() const
{
    return d->m_url;
}

QString AlkOnlineQuoteSource::idRegex() const
{
    return d->m_idRegex;
}

AlkOnlineQuoteSource::IdSelector AlkOnlineQuoteSource::idSelector() const
{
    return d->m_idSelector;
}

QString AlkOnlineQuoteSource::priceRegex() const
{
    return d->m_priceRegex;
}

AlkOnlineQuoteSource::DecimalSeparator AlkOnlineQuoteSource::priceDecimalSeparator() const
{
    return d->m_priceDecimalSeparator;
}

QString AlkOnlineQuoteSource::dateRegex() const
{
    return d->m_dateRegex;
}

AlkOnlineQuoteSource::DataFormat AlkOnlineQuoteSource::dataFormat() const
{
    return d->m_dataFormat;
}

QString AlkOnlineQuoteSource::dateFormat() const
{
    return d->m_dateFormat;
}

AlkOnlineQuoteSource::DownloadType AlkOnlineQuoteSource::downloadType() const
{
    return d->m_downloadType;
}

/**
 * Returns the name of the "Finance::Quote" source.
 * This function only makes sense if the current source
 * is of the specified type.
 *
 * @return "Finance::Quote" source name
 */
QString AlkOnlineQuoteSource::financeQuoteName() const
{
    return d->m_name.section(' ', 1);
}

void AlkOnlineQuoteSource::setName(const QString &name)
{
    d->m_name = name;
}

void AlkOnlineQuoteSource::setUrl(const QString &url)
{
    d->m_url = url;
}

void AlkOnlineQuoteSource::setPriceRegex(const QString &priceRegex)
{
    d->m_priceRegex = priceRegex;
}

void AlkOnlineQuoteSource::setPriceDecimalSeparator(DecimalSeparator separator)
{
    d->m_priceDecimalSeparator = separator;
}

void AlkOnlineQuoteSource::setIdRegex(const QString &idRegex)
{
    d->m_idRegex = idRegex;
}

void AlkOnlineQuoteSource::setIdSelector(AlkOnlineQuoteSource::IdSelector idSelector)
{
    d->m_idSelector = idSelector;
}

/**
 * Set regular expression for parsing dates
 *
 * An empty string as expression disables the extraction
 * of the date, which is sometimes necessary, for example
 * if the service does not provide a complete date.
 *
 * @param date regular expression
 */
void AlkOnlineQuoteSource::setDateRegex(const QString &dateRegex)
{
    d->m_dateRegex = dateRegex;
}

void AlkOnlineQuoteSource::setDataFormat(DataFormat dataFormat)
{
    d->m_dataFormat = dataFormat;
}

void AlkOnlineQuoteSource::setDateFormat(const QString &dateFormat)
{
    d->m_dateFormat = dateFormat;
}

void AlkOnlineQuoteSource::setDownloadType(DownloadType downloadType)
{
    d->m_downloadType = downloadType;
}

void AlkOnlineQuoteSource::setGHNS(bool state)
{
    d->m_storageChanged = d->m_isGHNSSource != state;
    d->m_isGHNSSource = state;
}

bool AlkOnlineQuoteSource::isGHNS() const
{
    return d->m_isGHNSSource;
}

bool AlkOnlineQuoteSource::isReadOnly() const
{
    return d->m_readOnly;
}

/**
 * Checks whether the current source is of type "Finance::Quote"
 *
 * @return state
 */
bool AlkOnlineQuoteSource::isFinanceQuote() const
{
    return d->m_name.contains(fqName);
}

/**
 * Checks whether the specified source name is of type "Finance::Quote"
 *
 * @return state
 */
bool AlkOnlineQuoteSource::isFinanceQuote(const QString &name)
{
    return name.contains(fqName);
}

QString AlkOnlineQuoteSource::ghnsWriteFileName() const
{
    return d->ghnsWriteFilePath();
}

void AlkOnlineQuoteSource::setProfile(AlkOnlineQuotesProfile *profile)
{
    d->m_profile = profile;
    qDebug() << "using profile" << profile->name();
}

AlkOnlineQuotesProfile *AlkOnlineQuoteSource::profile()
{
    return d->m_profile;
}

AlkOnlineQuotesProfile *AlkOnlineQuoteSource::profile() const
{
    return d->m_profile;
}

bool AlkOnlineQuoteSource::read()
{
    if (d->m_profile->hasGHNSSupport()) {
        if (d->readFromGHNSFile()) {
            return true;
        }
    }
    return d->read();
}

bool AlkOnlineQuoteSource::write()
{
    bool result = false;
    // check if type has been changedd->isGHNS
    if (d->m_profile->hasGHNSSupport() && d->m_isGHNSSource) {
        result = d->writeToGHNSFile();
        if (d->m_storageChanged)
            d->remove();
        return result;
    } else {
        if (d->m_storageChanged && d->m_profile->quoteSources().contains(d->m_name))
            d->m_name.append(".local");
        result = d->write();
        if (d->m_profile->hasGHNSSupport() && d->m_storageChanged) {
            d->removeGHNSFile();
        }
    }
    d->m_storageChanged = false;
    return result;
}

void AlkOnlineQuoteSource::rename(const QString &name)
{
    if (d->m_profile->type() != AlkOnlineQuotesProfile::Type::None) {
        remove();
        d->m_name = name;
        write();
    } else
        d->m_name = name;
}

void AlkOnlineQuoteSource::remove()
{
    if (d->m_profile->hasGHNSSupport() && d->m_isGHNSSource) {
        d->removeGHNSFile();
    } else if (d->m_profile->type() != AlkOnlineQuotesProfile::Type::None) {
        d->remove();
    }
}

const QString &AlkOnlineQuoteSource::defaultId() const
{
    return d->m_defaultId;
}

void AlkOnlineQuoteSource::setDefaultId(const QString &defaultId)
{
    d->m_defaultId = defaultId;
}

bool AlkOnlineQuoteSource::requiresTwoIdentifier() const
{
    return url().contains("%2");
}

AlkOnlineQuoteSource::Private::Private(const Private *other)
    : m_name(other->m_name)
    , m_url(other->m_url)
    , m_priceDecimalSeparator(other->m_priceDecimalSeparator)
    , m_priceRegex(other->m_priceRegex)
    , m_dataFormat(other->m_dataFormat)
    , m_dateRegex(other->m_dateRegex)
    , m_dateFormat(other->m_dateFormat)
    , m_downloadType(other->m_downloadType)
    , m_idRegex(other->m_idRegex)
    , m_idSelector(other->m_idSelector)
    , m_profile(other->m_profile)
    , m_isGHNSSource(other->m_isGHNSSource)
    , m_storageChanged(other->m_storageChanged)
    , m_readOnly(other->m_readOnly)
{
}
