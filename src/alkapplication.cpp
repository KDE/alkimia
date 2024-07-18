/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkapplication.h"

#include <KAboutData>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <KLocalizedString>
#include <QApplication>
#else
#include <KApplication>
#include <KCmdLineArgs>
#endif

class Private
{
public:
    int _argc{0};
    char **_argv{nullptr};
    QStringList _args;
    KAboutData *_aboutData{nullptr};
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QApplication *_app{nullptr};
#else
    KApplication *_app{nullptr};
#endif
    ~Private()
    {
        delete _aboutData;
    }
};

Q_GLOBAL_STATIC(Private, d)

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
AlkAboutData::AlkAboutData(const QString &componentName, const QString &displayName, const QString &version,
                           const QString &shortDescription, AlkAboutData::License license, const QString &copyrightStatement)
{
    KLocalizedString::setApplicationDomain(componentName.toStdString().c_str());
    d->_aboutData = new KAboutData(componentName, displayName, version, shortDescription, static_cast<KAboutLicense::LicenseKey>(license), copyrightStatement);
    d->_app = new QApplication(d->_argc, d->_argv);
}

AlkAboutData &AlkAboutData::setOrganizationDomain(const QByteArray &domain)
{
    d->_aboutData->setOrganizationDomain(domain);
    return *this;
}

void AlkAboutData::setApplicationData(AlkAboutData &aboutData)
{
    Q_UNUSED(aboutData);

    KAboutData::setApplicationData(*d->_aboutData);
}

QString AlkAboutData::displayName()
{
    return d()->_aboutData->displayName();
}
#else
AlkAboutData::AlkAboutData(const QString &componentName, const QString &displayName, const QString &version,
                           const QString &shortDescription, AlkAboutData::License license, const QString &copyrightStatement)
{
    d()->_aboutData = new KAboutData(componentName.toLatin1(), componentName.toLatin1(), ki18n(displayName.toStdString().c_str()),
                                     version.toStdString().c_str(), ki18n(shortDescription.toStdString().c_str()),
                                     static_cast<KAboutData::LicenseKey>(license), ki18n(copyrightStatement.toStdString().c_str()));
    KCmdLineArgs::init(d()->_aboutData);
    d()->_app = new KApplication(true);
}

AlkAboutData &AlkAboutData::setOrganizationDomain(const QByteArray &domain)
{
    return *this;
}

void AlkAboutData::setApplicationData(AlkAboutData &aboutData)
{
}

QString AlkAboutData::displayName()
{
    return d()->_aboutData->programName();
}
#endif

QString AlkAboutData::shortDescription() const
{
    return d()->_aboutData->shortDescription();
}

AlkApplication::AlkApplication(int argc, char** argv)
{
    d()->_argc = argc;
    d()->_argv = argv;
    for (int i = 0; i < argc; i++)
        d()->_args.append(argv[i]);
}

QStringList AlkApplication::arguments()
{
    return d()->_args;
}

int AlkApplication::exec()
{
    return d()->_app->exec();
}
