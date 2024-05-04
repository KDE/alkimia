/*
    SPDX-FileCopyrightText: 2023 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/


// #include <QtGlobal>
#include <QProcess>

#include <alkimia/alkenvironment.h>
#include "alkdebug.h"

void AlkEnvironment::checkForAppImageEnvironment(const char* applicationPath)
{
#ifdef Q_OS_UNIX
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (qEnvironmentVariableIsSet("APPDIR")) {
        QByteArray appFullPath(applicationPath);
        auto lastDirSeparator = appFullPath.lastIndexOf('/');
        auto appDir = appFullPath.left(lastDirSeparator + 1);
        auto appName = QString::fromUtf8(appFullPath.mid(lastDirSeparator + 1));
        alkDebug() << "AppImageInfo:" << appFullPath << appDir << appName;
        if (appName == QStringLiteral("AppRun.wrapped")) {
            appDir.append("usr/lib");
            const auto libPath = qgetenv("LD_LIBRARY_PATH");
            auto newLibPath = appDir;
            if (!libPath.isEmpty()) {
                newLibPath.append(':');
                newLibPath.append(libPath);
            }
            qputenv("RUNNING_AS_APPIMAGE", "true");
            qputenv("LD_LIBRARY_PATH", newLibPath);
            alkDebug() << "LD_LIBRARY_PATH set to" << newLibPath;
        }
    }
    #endif
#endif
}

bool AlkEnvironment::isRunningAsAppImage()
{
#ifdef Q_OS_UNIX
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return qEnvironmentVariableIsSet("RUNNING_AS_APPIMAGE");
    #else
    return false;
    #endif
#else
    return false;
#endif
}


void AlkEnvironment::removeAppImagePathFromLinkLoaderLibPath(QProcess* process)
{
#ifdef Q_OS_UNIX
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    if (isRunningAsAppImage() && process) {
        auto environment = QProcessEnvironment::systemEnvironment();
        auto ld_library_path = environment.value(QLatin1String("LD_LIBRARY_PATH"));
        if (!ld_library_path.isEmpty()) {
            const auto appdir = environment.value(QLatin1String("APPDIR"));
            auto path_list = ld_library_path.split(QLatin1Char(':'));
            while (!path_list.isEmpty() && path_list.at(0).startsWith(appdir)) {
                path_list.removeAt(0);
                ld_library_path.clear();
                if (!path_list.isEmpty()) {
                    ld_library_path = path_list.join(QLatin1Char(':'));
                }
                if (!ld_library_path.isEmpty()) {
                    environment.insert(QLatin1String("LD_LIBRARY_PATH"), ld_library_path);
                } else {
                    environment.remove(QLatin1String("LD_LIBRARY_PATH"));
                }
                process->setProcessEnvironment(environment);
            }
        }
    }
    #endif
#endif
}
