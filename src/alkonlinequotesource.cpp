/*
 * SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker@freenet.de
 * SPDX-FileCopyrightText: 2023 Thomas Baumgart <tbaumgart@kde.org>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of libalkimia.
 */

#include "alkonlinequotesource.h"

#include "alkonlinequotesprofile.h"

#include <QFile>
#include <QFileInfo>
#include <QtDebug>

#include <KConfig>
#include <KConfigGroup>

/**
 * Key to identifying "Finance::Quote" sources
 */
static const char *fqName = "Finance::Quote";

class AlkOnlineQuoteSource::Private
{
public:
    Private()
        : m_idSelector(Symbol)
        , m_skipStripping(false)
        , m_profile(nullptr)
        , m_isGHNSSource(false)
        , m_storageChanged(false)
        , m_readOnly(true)
    {
    }

    Private(const Private* other)
        : m_name(other->m_name)
        , m_url(other->m_url)
        , m_sym(other->m_sym)
        , m_price(other->m_price)
        , m_date(other->m_date)
        , m_dateformat(other->m_dateformat)
        , m_idNumber(other->m_idNumber)
        , m_idSelector(other->m_idSelector)
        , m_skipStripping(other->m_skipStripping)
        , m_profile(other->m_profile)
        , m_isGHNSSource(other->m_isGHNSSource)
        , m_storageChanged(other->m_storageChanged)
        , m_readOnly(other->m_readOnly)
    {
    }

    bool read()
    {
        KConfig *kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        const QString &group = QString("Online-Quote-Source-%1").arg(m_name);
        if (!kconfig->hasGroup(group)) {
            return false;
        }
        KConfigGroup grp = kconfig->group(group);
        m_sym = grp.readEntry("SymbolRegex");
        m_date = grp.readEntry("DateRegex");
        m_dateformat = grp.readEntry("DateFormatRegex", "%m %d %y");
        m_price = grp.readEntry("PriceRegex");
        m_idNumber = grp.readEntry("IDRegex");
        m_idSelector = static_cast<IdSelector>(grp.readEntry("IDBy", "0").toInt());
        m_url = grp.readEntry("URL");
        m_skipStripping = grp.readEntry("SkipStripping", false);
        m_isGHNSSource = false;
        m_readOnly = false;
        return true;
    }

    bool write()
    {
        KConfig *kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        KConfigGroup grp = kconfig->group(QString("Online-Quote-Source-%1").arg(m_name));
        grp.writeEntry("URL", m_url);
        grp.writeEntry("PriceRegex", m_price);
        grp.writeEntry("DateRegex", m_date);
        grp.writeEntry("DateFormatRegex", m_dateformat);
        grp.writeEntry("IDRegex", m_idNumber);
        grp.writeEntry("IDBy", static_cast<int>(m_idSelector));
        grp.writeEntry("SymbolRegex", m_sym);
        if (m_skipStripping) {
            grp.writeEntry("SkipStripping", m_skipStripping);
        } else {
            grp.deleteEntry("SkipStripping");
        }
        kconfig->sync();
        return true;
    }

    bool remove()
    {
        KConfig *kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        kconfig->deleteGroup(QString("Online-Quote-Source-%1").arg(m_name));
        kconfig->sync();
        return true;
    }

    QString ghnsReadFilePath()
    {
        return m_profile->hotNewStuffReadFilePath(m_name + QLatin1String(".txt"));
    }

    QString ghnsWriteFilePath()
    {
        return m_profile->hotNewStuffWriteFilePath(m_name + QLatin1String(".txt"));
    }

    // This is currently in skrooge format
    bool readFromGHNSFile()
    {
        QFileInfo f(ghnsReadFilePath());
        if (!f.exists())
            f.setFile(ghnsWriteFilePath());
        m_readOnly = !f.isWritable();

        QFile file(f.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            int index = line.indexOf("=");
            if (index == -1)
                return false;
            QString key = line.left(index);
            QString value = line.mid(index+1);
            if (key == "url")
                m_url = value;
            else if (key == "price") {
                m_price = value;
                m_price.replace("\\\\", "\\");
            } else if (key == "date") {
                m_date = value;
                m_date.replace("\\\\", "\\");
            } else if (key == "dateformat")
                m_dateformat = value;
        }

        m_skipStripping = true;
        m_isGHNSSource = true;
        return true;
    }

    // This is currently in skrooge format
    bool writeToGHNSFile()
    {
        QFile file(ghnsWriteFilePath());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        out << "date=" << m_date << "\n";
        out << "dateformat=" << m_dateformat << "\n";
        out << "mode=HTML\n";
        out << "price=" << m_price << "\n";
        out << "url=" << m_url << "\n";
        return true;
    }

    bool removeGHNSFile()
    {
        qDebug() << "delete" << ghnsWriteFilePath();
        return true;
    }

    QString m_name;
    QString m_url;
    QString m_sym;
    QString m_price;
    QString m_date;
    QString m_dateformat;
    QString m_idNumber;
    IdSelector m_idSelector;
    bool m_skipStripping;
    AlkOnlineQuotesProfile *m_profile;
    bool m_isGHNSSource;
    bool m_storageChanged;
    bool m_readOnly;
};

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
                                           const QString& sym,
                                           const QString& idNumber,
                                           const IdSelector idBy,
                                           const QString& price,
                                           const QString& date,
                                           const QString& dateformat,
                                           bool skipStripping)
    : d(new Private)
{
    d->m_name = name;
    d->m_url = url;
    d->m_sym = sym;
    d->m_idNumber = idNumber;
    d->m_idSelector = idBy;
    d->m_price = price;
    d->m_date = date;
    d->m_dateformat = dateformat;
    d->m_skipStripping = skipStripping;
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
                                QString(), // symbolregexp
                                QString(), // idregexp
                                AlkOnlineQuoteSource::Symbol,
                                "Today\\s+=\\s+([^<]+)",
                                "name=\"date_input\" class=\"ip_ondate\" value=\"(\\d{4}-\\d{2}-\\d{2})",
                                "%y/%m/%d",
                                true // skip HTML stripping
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

QString AlkOnlineQuoteSource::sym() const
{
    return d->m_sym;
}

QString AlkOnlineQuoteSource::idNumber() const
{
    return d->m_idNumber;
}

AlkOnlineQuoteSource::IdSelector AlkOnlineQuoteSource::idSelector() const
{
    return d->m_idSelector;
}

QString AlkOnlineQuoteSource::price() const
{
    return d->m_price;
}

QString AlkOnlineQuoteSource::date() const
{
    return d->m_date;
}

QString AlkOnlineQuoteSource::dateformat() const
{
    return d->m_dateformat;
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

bool AlkOnlineQuoteSource::skipStripping() const
{
    return d->m_skipStripping;
}

void AlkOnlineQuoteSource::setName(const QString &name)
{
    d->m_name = name;
}

void AlkOnlineQuoteSource::setUrl(const QString &url)
{
    d->m_url = url;
}

void AlkOnlineQuoteSource::setSym(const QString &symbol)
{
    d->m_sym = symbol;
}

void AlkOnlineQuoteSource::setPrice(const QString &price)
{
    d->m_price = price;
}

void AlkOnlineQuoteSource::setIdNumber(const QString &idNumber)
{
    d->m_idNumber = idNumber;
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
void AlkOnlineQuoteSource::setDate(const QString &date)
{
    d->m_date = date;
}

void AlkOnlineQuoteSource::setDateformat(const QString &dateformat)
{
    d->m_dateformat = dateformat;
}

void AlkOnlineQuoteSource::setSkipStripping(bool state)
{
    d->m_skipStripping = state;
}

void AlkOnlineQuoteSource::setGHNS(bool state)
{
    d->m_storageChanged = d->m_isGHNSSource != state;
    d->m_isGHNSSource = state;
}

bool AlkOnlineQuoteSource::isGHNS()
{
    return d->m_isGHNSSource;
}

bool AlkOnlineQuoteSource::isReadOnly()
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

QString AlkOnlineQuoteSource::ghnsWriteFileName()
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

bool AlkOnlineQuoteSource::read()
{
    bool result = false;
    if (d->m_profile->hasGHNSSupport()) {
        result = d->readFromGHNSFile();
        if (result)
            return true;
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
