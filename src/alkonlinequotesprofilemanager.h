/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESPROFILEMANAGER_H
#define ALKONLINEQUOTESPROFILEMANAGER_H

#include <alkimia/alkonlinequotesprofile.h>

#include <QList>
#include <QStringList>

class AlkWebPage;

typedef QList<AlkOnlineQuotesProfile *> AlkOnlineQuotesProfileList;

class ALK_EXPORT AlkOnlineQuotesProfileManager : public QObject
{
    Q_OBJECT
public:
    ~AlkOnlineQuotesProfileManager();

    void addProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfileList profiles();
    AlkOnlineQuotesProfile *profile(const QString &name);
    QStringList profileNames();
    AlkWebPage *webPage();
    void setWebPageEnabled(bool enable);
    bool webPageEnabled();

    static AlkOnlineQuotesProfileManager &instance();

Q_SIGNALS:
    /**
     * emit quote source for which an update is available
     */
    void updateAvailable(const QString &profile, const QString &name);

private:
    AlkOnlineQuotesProfileManager();
    class Private;
    Private *const d;
};

#endif // ALKONLINEQUOTESPROFILEMANAGER_H
