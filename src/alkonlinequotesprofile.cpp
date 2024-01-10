/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesprofile.h"

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"

#include "alkonlinequotesprofile_p.h"

#include <KSharedConfig>
#include <QtDebug>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <KNSCore/Engine>
    #include <QRegularExpression>
    #include <QStandardPaths>
#else
    #include <QRegExp>
    #include <KConfig>
    #include <KGlobal>
    #include <KStandardDirs>
#endif

AlkOnlineQuotesProfile::AlkOnlineQuotesProfile(const QString &name, Type type,
                                               const QString &ghnsConfigFile)
    : d(new Private(this))
{
    d->m_name = name;
    d->m_GHNSFile = ghnsConfigFile;
    d->m_type = type;
    if (type == Type::KMyMoney5)
        d->m_kconfigFile = QString("%1/kmymoney/kmymoneyrc").arg(d->configPath());
    else if (type == Type::KMyMoney4)
        d->m_kconfigFile = QString("%1/kmymoneyrc").arg(d->configPath());
    else if (type == Type::Alkimia5 || type == Type::Alkimia4)
        d->m_kconfigFile = QString("%1/alkimiarc").arg(d->configPath());
    else
        d->m_kconfigFile.clear();

    if (!d->m_kconfigFile.isEmpty()) {
        d->m_config = KSharedConfig::openConfig(d->m_kconfigFile, KConfig::SimpleConfig);
        qDebug() << d->m_config;
    }

    if (!d->m_GHNSFile.isEmpty()) {
        KConfig ghnsFile(hotNewStuffConfigFile());
        KConfigGroup group = ghnsFile.group("KNewStuff3");
        d->m_GHNSFilePath = group.readEntry("TargetDir");
        d->checkUpdates();
    }
}

AlkOnlineQuotesProfile::~AlkOnlineQuotesProfile()
{
    delete d;
}

QString AlkOnlineQuotesProfile::name() const
{
    return d->m_name;
}

QString AlkOnlineQuotesProfile::hotNewStuffConfigFile() const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QString configFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, d->m_GHNSFile);
    if (configFile.isEmpty()) {
        configFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "knsrcfiles/" + d->m_GHNSFile);
    }
#else
    QString configFile = KStandardDirs::locate("config", d->m_GHNSFile);
    if (configFile.isEmpty()) {
         configFile = KStandardDirs::locate("data", "knsrcfiles/" + d->m_GHNSFile);
    }
#endif
    if (configFile.isEmpty()) {
        configFile = QString("%1/%2").arg(KNSRC_DIR, d->m_GHNSFile);
    }

    return configFile;
}

QString AlkOnlineQuotesProfile::hotNewStuffReadFilePath(const QString &fileName) const
{
    for (const QString &path : hotNewStuffReadPath()) {
        QFileInfo f(path + fileName);
        if (f.exists())
            return f.absoluteFilePath();
    }
    return QString();
}

QString AlkOnlineQuotesProfile::hotNewStuffWriteFilePath(const QString &fileName) const
{
    return QString("%1%2").arg(hotNewStuffWriteDir(), fileName);
}

QStringList AlkOnlineQuotesProfile::hotNewStuffReadPath() const
{
    return QStringList()
        << QString("%1/%2/").arg(d->dataReadPath(), d->m_GHNSFilePath)
        << hotNewStuffWriteDir();
}

QString AlkOnlineQuotesProfile::hotNewStuffWriteDir() const
{
    return QString("%1/%2/").arg(d->dataWritePath(), d->m_GHNSFilePath);
}

QString AlkOnlineQuotesProfile::hotNewStuffRelPath() const
{
    return d->m_GHNSFilePath;
}

QString AlkOnlineQuotesProfile::kConfigFile() const
{
    return d->m_kconfigFile;
}

KSharedConfigPtr AlkOnlineQuotesProfile::kConfig() const
{
    return d->m_config;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void AlkOnlineQuotesProfile::setKConfig(KSharedConfigPtr kconfig)
{
    d->m_config = kconfig;
}
#endif

AlkOnlineQuotesProfile::Type AlkOnlineQuotesProfile::type()
{
    return d->m_type;
}

bool AlkOnlineQuotesProfile::typeIsSupported() const
{
#ifndef ENABLE_FINANCEQUOTE
    if (d->m_type == Type::Script) {
        return false;
    }
#endif
    return true;
}

bool AlkOnlineQuotesProfile::typeIsOperational() const
{
    if (d->m_type == Type::Script) {
#ifdef ENABLE_FINANCEQUOTE
        if (d->quoteSourcesFinanceQuote().isEmpty()) {
            return false;
        }
#else
        return false;
#endif
    }
    return true;
}

bool AlkOnlineQuotesProfile::hasGHNSSupport()
{
    return !d->m_GHNSFile.isEmpty();
}

const AlkOnlineQuotesProfile::Map AlkOnlineQuotesProfile::defaultQuoteSources()
{
    return d->defaultQuoteSources();
}

const QStringList AlkOnlineQuotesProfile::quoteSources()
{
    QStringList result;
    switch(d->m_type) {
    case AlkOnlineQuotesProfile::Type::Alkimia4:
    case AlkOnlineQuotesProfile::Type::Alkimia5:
    case AlkOnlineQuotesProfile::Type::KMyMoney4:
    case AlkOnlineQuotesProfile::Type::KMyMoney5:
        result << d->quoteSourcesNative();
        break;
#ifdef ENABLE_FINANCEQUOTE
    case AlkOnlineQuotesProfile::Type::Script:
        result << d->quoteSourcesFinanceQuote();
        break;
#endif
    case AlkOnlineQuotesProfile::Type::None:
        result << d->defaultQuoteSources().keys();
        break;
    default:
        break;
    }
    if (hasGHNSSupport())
        result << d->quoteSourcesGHNS();
    return result;
}

void AlkOnlineQuotesProfile::setManager(AlkOnlineQuotesProfileManager *manager)
{
    d->m_profileManager = manager;
}

AlkOnlineQuotesProfileManager *AlkOnlineQuotesProfile::manager()
{
    return d->m_profileManager;
}

QString AlkOnlineQuotesProfile::scriptPath()
{
    return d->m_financeQuoteScriptPath;
}
