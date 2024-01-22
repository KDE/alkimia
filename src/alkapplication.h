/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKAPPLICATION_H
#define ALKAPPLICATION_H

#include <QStringList>

class AlkAboutData
{
public:
    enum License {License_GPL = 1, License_LGPL = 2 };
    explicit AlkAboutData(const QString &componentName = {}, const QString &displayName = {}, const QString &version = {},
                          const QString &shortDescription = {}, AlkAboutData::License license = {}, const QString &copyrightStatement = {});
    AlkAboutData &setOrganizationDomain(const QByteArray &domain);
    static void setApplicationData(AlkAboutData &aboutData);
    QString displayName();
    QString shortDescription() const;
};

class AlkApplication
{
public:
    AlkApplication(int argc, char** argv);
    QStringList arguments();
    int exec();
};

#endif // ALKAPPLICATION_H
