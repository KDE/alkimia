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

#include "alkonlinequotesource.h"

#include "alkonlinequotesprofile.h"

#include <QFileInfo>
#include <QtDebug>

#include <KConfig>
#include <KConfigGroup>

class AlkOnlineQuoteSource::Private
{
public:
    Private()
        : m_skipStripping(false)
        , m_profile(nullptr)
        , m_isGHNSSource(false)
        , m_storageChanged(false)
    {
    }

    Private(const Private *other)
        : m_name(other->m_name)
        , m_url(other->m_url)
        , m_sym(other->m_sym)
        , m_price(other->m_price)
        , m_date(other->m_date)
        , m_dateformat(other->m_dateformat)
        , m_skipStripping(other->m_skipStripping)
        , m_profile(other->m_profile)
        , m_isGHNSSource(other->m_isGHNSSource)
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
        m_url = grp.readEntry("URL");
        m_skipStripping = grp.readEntry("SkipStripping", false);
        m_isGHNSSource = false;
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
        QString fileName = m_profile->hotNewStuffReadFilePath(m_name);
        if (!fileName.endsWith(".txt"))
            fileName.append(".txt");
        return fileName;
    }

    QString ghnsWriteFilePath()
    {
        QString fileName = m_profile->hotNewStuffWriteFilePath(m_name);
        if (!fileName.endsWith(".txt"))
            fileName.append(".txt");
        return fileName;
    }

    // This is currently in skrooge format
    bool readFromGHNSFile()
    {
        QFileInfo f(ghnsReadFilePath());
        if (!f.exists())
            f.setFile(ghnsWriteFilePath());
        KConfig config(f.absoluteFilePath());
        KConfigGroup group(&config, "<default>");
        if (!(group.hasKey("mode") && group.readEntry("mode") == "HTML"
              && group.hasKey("url") && group.hasKey("date")
              && group.hasKey("dateformat") && group.hasKey("price"))) {
            return false;
        }
        m_skipStripping = true;
        m_url = group.readEntry("url");
        m_price = group.readEntry("price");
        m_date = group.readEntry("date");
        m_dateformat = group.readEntry("dateformat");
        m_isGHNSSource = true;
        return true;
    }

    // This is currently in skrooge format
    bool writeToGHNSFile()
    {
        KConfig config(ghnsWriteFilePath());
        KConfigGroup group(&config, "<default>");
        group.writeEntry("url", m_url);
        group.writeEntry("price", m_price);
        group.writeEntry("date", m_date);
        group.writeEntry("dateformat", m_dateformat);
        group.writeEntry("mode", "HTML");
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
    bool m_skipStripping;
    AlkOnlineQuotesProfile *m_profile;
    bool m_isGHNSSource;
    bool m_storageChanged;
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

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const QString &name, const QString &url,
                                           const QString &sym, const QString &price,
                                           const QString &date, const QString &dateformat,
                                           bool skipStripping)
    : d(new Private)
{
    d->m_name = name;
    d->m_url = url;
    d->m_sym = sym;
    d->m_price = price;
    d->m_date = date;
    d->m_dateformat = dateformat;
    d->m_skipStripping = skipStripping;
    d->m_isGHNSSource = false;
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const QString &name, AlkOnlineQuotesProfile *profile)
    : d(new Private)
{
    if (profile->type() == AlkOnlineQuotesProfile::Type::None && profile->defaultQuoteSources().contains(name)) {
        *this = profile->defaultQuoteSources()[name];
    } else {
        d->m_profile = profile;
        d->m_name = name;
        read();
    }
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
