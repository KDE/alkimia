/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESPROFILE_H
#define ALKONLINEQUOTESPROFILE_H

#include <alkimia/alk_export.h>
#include <alkimia/alkonlinequotesource.h>

#include <KSharedConfig>

#include <QMap>
#include <QObject>
#include <QString>

class AlkOnlineQuoteSource;
class AlkOnlineQuotesProfileManager;

class ALK_EXPORT AlkOnlineQuotesProfile : public QObject
{
    Q_OBJECT
public:
    typedef QMap<QString, AlkOnlineQuoteSource> Map;
    enum class Type {
        Undefined, None, Alkimia4, Alkimia5, KMyMoney4, KMyMoney5, Skrooge4, Skrooge5, Script, Test
    };
    explicit AlkOnlineQuotesProfile(const QString &name = "alkimia", Type type = Type::None,
        const QString &ghnsConfigFile = QString());
    ~AlkOnlineQuotesProfile();

    QString name() const;
    QString hotNewStuffConfigFile() const;
    QString hotNewStuffRelPath() const;
    QStringList hotNewStuffReadPath() const;
    QString hotNewStuffWriteDir() const;
    QString hotNewStuffReadFilePath(const QString &fileName) const;
    QString hotNewStuffWriteFilePath(const QString &fileName) const;
    QString kConfigFile() const;
    void setManager(AlkOnlineQuotesProfileManager *manager);
    AlkOnlineQuotesProfileManager *manager();

    KSharedConfigPtr kConfig() const;
    void setKConfig(KSharedConfigPtr kconfig);

    /**
     * Check if the profile is supported (build) by Alkimia
     *
     * @returns @c true if support is compiled, @c false otherwise.
     */
    bool typeIsSupported() const;

    /**
     * Check if the profile is functional
     *
     * @returns @c true if support is available, @c false otherwise.
     */
    bool typeIsOperational() const;

    Type type();
    bool hasGHNSSupport();

    /**
     * return available default sources of this profile
     */
    const Map defaultQuoteSources();

    /**
     * return available sources of this profile
     */
    const QStringList quoteSources();

    /**
     * return testing sources from this profile
     */
    const Map quoteSourcesTesting();

    QString scriptPath();

    QString GHNSId(const QString &name) const;
    QString GHNSFilePath(const QString &name) const;
    QString GHNSName(const QString &id) const;

    void reload();

Q_SIGNALS:
    /**
     * emit quote source for which an update is available
     */
   void updateAvailable(const QString &profile, const QString &name);

    /**
     * emit that the sources from this profile has been changed
     */
    void sourcesChanged();

private:
    class Private;
    Private *const d;
};

#endif // ALKONLINEQUOTESPROFILE_H
