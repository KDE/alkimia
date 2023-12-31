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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <KSharedConfig>
#else
#include <KConfig>
#endif
#include <KConfigGroup>

/**
 * Key to identifying "Finance::Quote" sources
 */
static const char *fqName = "Finance::Quote";

class AlkOnlineQuoteSource::Private
{
public:
    Private()
        : m_dataFormat(HTML)
        , m_idSelector(Symbol)
        , m_profile(nullptr)
        , m_isGHNSSource(false)
        , m_storageChanged(false)
        , m_readOnly(true)
    {
    }

    explicit Private(const Private* other)
        : m_name(other->m_name)
        , m_url(other->m_url)
        , m_priceRegex(other->m_priceRegex)
        , m_dataFormat(other->m_dataFormat)
        , m_dateRegex(other->m_dateRegex)
        , m_dateFormat(other->m_dateFormat)
        , m_idRegex(other->m_idRegex)
        , m_idSelector(other->m_idSelector)
        , m_profile(other->m_profile)
        , m_isGHNSSource(other->m_isGHNSSource)
        , m_storageChanged(other->m_storageChanged)
        , m_readOnly(other->m_readOnly)
    {
    }

    bool read()
    {
        auto kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        const QString &group = QString("Online-Quote-Source-%1").arg(m_name);
        if (!kconfig->hasGroup(group)) {
            return false;
        }
        KConfigGroup grp = kconfig->group(group);
        // in newer profiles DataFormat takes precedence over SkipStripping
        // if DataFormat is not present, search for SkipStripping
        if (grp.hasKey(QLatin1String("DataFormat"))) {
            m_dataFormat = static_cast<DataFormat>(grp.readEntry(QLatin1String("DataFormat"), 0));
        } else {
            m_dataFormat = grp.readEntry<bool>(QLatin1String("SkipStripping"), false) == true ? HTML : StrippedHTML;
        }

        m_dateRegex = grp.readEntry("DateRegex");
        m_dateFormat = grp.readEntry("DateFormatRegex", "%m %d %y");
        if (grp.hasKey("DefaultId"))
            m_defaultId = grp.readEntry("DefaultId");
        else if (grp.hasKey("DebugId")) // For compatibility with 8.1.72
            m_defaultId = grp.readEntry("DebugId");
        m_priceRegex = grp.readEntry("PriceRegex");
        if (grp.hasKey("SymbolRegex"))
            m_idRegex = grp.readEntry("SymbolRegex");
        else
            m_idRegex = grp.readEntry("IDRegex");
        m_idSelector = static_cast<IdSelector>(grp.readEntry("IDBy", "0").toInt());
        m_url = grp.readEntry("URL");

        m_isGHNSSource = false;
        m_readOnly = false;
        return true;
    }

    bool write()
    {
        auto kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        KConfigGroup grp = kconfig->group(QString("Online-Quote-Source-%1").arg(m_name));
        grp.writeEntry("URL", m_url);
        grp.writeEntry("PriceRegex", m_priceRegex);
        grp.writeEntry("DataFormat", static_cast<int>(m_dataFormat));
        grp.writeEntry("DateRegex", m_dateRegex);
        grp.writeEntry("DateFormatRegex", m_dateFormat);
        grp.deleteEntry("DebugId");
        grp.writeEntry("DefaultId", m_defaultId);
        grp.writeEntry("IDRegex", m_idRegex);
        grp.writeEntry("IDBy", static_cast<int>(m_idSelector));
        /// @todo remove the following code block if backward
        /// compatibility is not needed anymore. for some time,
        /// we maintain it though
        if (m_dataFormat == HTML) {
            grp.writeEntry(QLatin1String("SkipStripping"), true);
        } else {
            grp.deleteEntry("SkipStripping");
        }
        grp.deleteEntry("SymbolRegex");


        kconfig->sync();
        return true;
    }

    bool remove()
    {
        auto kconfig = m_profile->kConfig();
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
                m_priceRegex = value;
                m_priceRegex.replace("\\\\", "\\");
            } else if (key == "date") {
                m_dateRegex = value;
                m_dateRegex.replace("\\\\", "\\");
            } else if (key == "dateformat")
                m_dateFormat = value;
            else if (key == "defaultid")
                m_defaultId = value;
            else if (key == "debugid") // for compatibility with 8.1.72
                m_defaultId = value;
            else if (key == "mode") {
                if (value == "StrippedHTML")
                    m_dataFormat = AlkOnlineQuoteSource::DataFormat::StrippedHTML;
                else if (value == "HTML")
                    m_dataFormat = AlkOnlineQuoteSource::DataFormat::HTML;
                else if (value == "CSV")
                    m_dataFormat = AlkOnlineQuoteSource::DataFormat::CSV;
            }
        }

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
        out << "date=" << m_dateRegex << "\n";
        out << "dateformat=" << m_dateFormat << "\n";
        out << "defaultid=" << m_defaultId << "\n";
        if (m_dataFormat == AlkOnlineQuoteSource::DataFormat::StrippedHTML)
            out << "mode=StrippedHTML\n";
        else if (m_dataFormat == AlkOnlineQuoteSource::DataFormat::HTML)
            out << "mode=HTML\n";
        else if (m_dataFormat == AlkOnlineQuoteSource::DataFormat::CSV)
            out << "mode=CSV\n";
        out << "price=" << m_priceRegex << "\n";
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
    QString m_priceRegex;
    DataFormat m_dataFormat;
    QString m_dateRegex;
    QString m_dateFormat;
    QString m_defaultId;
    QString m_idRegex;
    IdSelector m_idSelector;
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
                                           const QString& idRegex,
                                           const IdSelector idBy,
                                           const QString& priceRegex,
                                           const QString& dateRegex,
                                           const QString& dateFormat,
                                           DataFormat dataFormat)
    : d(new Private)
{
    d->m_name = name;
    d->m_url = url;
    d->m_idRegex = idRegex;
    d->m_idSelector = idBy;
    d->m_priceRegex = priceRegex;
    d->m_dataFormat = dataFormat;
    d->m_dateRegex = dateRegex;
    d->m_dateFormat = dateFormat;
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
        if (d->m_profile->quoteSources().contains(d->m_name))
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
