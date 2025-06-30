/*
 * SPDX-FileCopyrightText: 2018,2023 Ralf Habacker ralf.habacker@freenet.de
 * SPDX-FileCopyrightText: 2023 Thomas Baumgart <tbaumgart@kde.org>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of libalkimia.
 */

#ifndef ALKONLINEQUOTESOURCE_P_H
#define ALKONLINEQUOTESOURCE_P_H

#include "alkonlinequotesource.h"

#include "alkdebug.h"

#include <QFile>
#include <QFileInfo>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <KSharedConfig>
#else
#include <KConfig>
#endif
#include <KConfigGroup>

class ALK_NO_EXPORT AlkOnlineQuoteSource::Private
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

    explicit Private(const Private* other);

    bool read()
    {
        if (!m_profile)
            return false;
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

        m_referenceId = grp.readEntry("ReferenceId");
        m_dateRegex = grp.readEntry("DateRegex");
        m_dateFormat = grp.readEntry("DateFormatRegex", "%m %d %y");
        if (grp.hasKey("DefaultId"))
            m_defaultId = grp.readEntry("DefaultId");
        else if (grp.hasKey("DebugId")) // For compatibility with 8.1.72
            m_defaultId = grp.readEntry("DebugId");
        m_downloadType = static_cast<DownloadType>(grp.readEntry("DownloadType", QString("%1").arg(static_cast<int>(Default))).toInt());
        m_priceDecimalSeparator = static_cast<DecimalSeparator>(grp.readEntry("PriceDecimalSeparator", QString("%1").arg(static_cast<int>(Legacy))).toInt());
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
        if (!m_profile)
            return false;
        auto kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        KConfigGroup grp = kconfig->group(QString("Online-Quote-Source-%1").arg(m_name));
        grp.writeEntry("ReferenceId", m_referenceId);
        grp.writeEntry("URL", m_url);
        grp.writeEntry("PriceDecimalSeparator", static_cast<int>(m_priceDecimalSeparator));
        grp.writeEntry("PriceRegex", m_priceRegex);
        grp.writeEntry("DataFormat", static_cast<int>(m_dataFormat));
        grp.writeEntry("DateRegex", m_dateRegex);
        grp.writeEntry("DateFormatRegex", m_dateFormat);
        grp.deleteEntry("DebugId");
        grp.writeEntry("DefaultId", m_defaultId);
        grp.writeEntry("DownloadType", static_cast<int>(m_downloadType));
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
        if (!m_profile)
            return false;
        auto kconfig = m_profile->kConfig();
        if (!kconfig)
            return false;
        kconfig->deleteGroup(QString("Online-Quote-Source-%1").arg(m_name));
        kconfig->sync();
        return true;
    }

    QString ghnsReadFilePath()
    {
        if (!m_profile)
            return QString();
        QString file = m_profile->GHNSFilePath(m_name);
        if (!file.isEmpty())
            return file;
        return m_profile->hotNewStuffReadFilePath(m_name + QLatin1String(".txt"));
    }

    QString ghnsWriteFilePath()
    {
        if (!m_profile)
            return QString();
        QString file = m_profile->GHNSFilePath(m_name);
        if (!file.isEmpty())
            return file;
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
            else if (key == "downloadtype") {
                if (value == "Default")
                    m_downloadType = AlkOnlineQuoteSource::Default;
                else if (value == "Javascript")
                    m_downloadType = AlkOnlineQuoteSource::Javascript;
            } else if (key == "pricedecimalseparator") {
                if (value == "Legacy")
                    m_priceDecimalSeparator = AlkOnlineQuoteSource::Legacy;
                else if (value == "Period")
                    m_priceDecimalSeparator = AlkOnlineQuoteSource::Period;
                else if (value == "Comma")
                    m_priceDecimalSeparator = AlkOnlineQuoteSource::Comma;
            } else if (key == "price") {
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
                else if (value == "CSS")
                    m_dataFormat = AlkOnlineQuoteSource::DataFormat::CSS;
                else if (value == "JSON")
                    m_dataFormat = AlkOnlineQuoteSource::DataFormat::JSON;
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
        else if (m_dataFormat == AlkOnlineQuoteSource::DataFormat::CSS)
            out << "mode=CSS\n";
        else if (m_dataFormat == AlkOnlineQuoteSource::DataFormat::JSON)
            out << "mode=JSON\n";
        if (m_downloadType == AlkOnlineQuoteSource::Default)
            out << "downloadtype=Default\n";
        else if (m_downloadType == AlkOnlineQuoteSource::Javascript)
            out << "downloadtype=Javascript\n";
        if (m_priceDecimalSeparator == AlkOnlineQuoteSource::Legacy)
            out << "pricedecimalseparator=Legacy\n";
        else if (m_priceDecimalSeparator == AlkOnlineQuoteSource::Period)
            out << "pricedecimalseparator=Period\n";
        else if (m_priceDecimalSeparator == AlkOnlineQuoteSource::Comma)
            out << "pricedecimalseparator=Comma\n";
        out << "price=" << m_priceRegex << "\n";
        out << "url=" << m_url << "\n";
        return true;
    }

    bool removeGHNSFile()
    {
        alkDebug() << "delete" << ghnsWriteFilePath();
        return QFile::remove(ghnsWriteFilePath());
    }

    QString m_referenceId;
    QString m_name;
    QString m_url;
    DecimalSeparator m_priceDecimalSeparator{Legacy};
    QString m_priceRegex;
    DataFormat m_dataFormat;
    QString m_dateRegex;
    QString m_dateFormat;
    QString m_defaultId;
    DownloadType m_downloadType{Default};
    QString m_idRegex;
    IdSelector m_idSelector;
    AlkOnlineQuotesProfile *m_profile;
    bool m_isGHNSSource;
    bool m_storageChanged;
    bool m_readOnly;
};

#endif // ALKONLINEQUOTESOURCE_P_H
