/*
    SPDX-FileCopyrightText: 2023 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKENVIRONMENT_H
#define ALKENVIRONMENT_H

#include <alkimia/alk_export.h>

class QProcess;

class ALK_EXPORT AlkEnvironment
{
public:
    /**
     * Check if @a applicationPath (usually argv[0]) is pointing to an application
     * inside an AppImage container and setting up certain environment variables
     *
     * For AppImages we need to set the LD_LIBRARY_PATH to have
     * $APPDIR/usr/lib/ as the first entry. We set this up here.
     * For security reasons, we extract the directory from argv[0]
     * and don't use APPDIR directly. It would otherwise allow to
     * add a different library path for non AppImage versions.
     *
     * Has no function if not running inside AppImage container or
     * Q_OS_UNIX is not defined
     */
    static void checkForAppImageEnvironment(const char* applicationPath);

    /**
     * Check if the current application is running inside an AppImage
     * container.
     *
     * @return true Running inside AppImage container
     * @return false otherwise
     *
     * @note Requires checkForAppImageEnvironment() to be run before
     */
    static bool isRunningAsAppImage();

    /**
     * Adjust the LD_LIBRARY_PATH environment variable to exclude
     * the AppImage mount point so that external tools do not try
     * to use libs contained inside the AppImage for the @a process
     * pointed to by the argument.
     *
     * Has no function if not running inside AppImage container,
     * Q_OS_UNIX is not defined or @a process is @c nullptr
     *
     * @note Requires checkForAppImageEnvironment() to be run before
     */
    static void removeAppImagePathFromLinkLoaderLibPath(QProcess* process);
};
#endif // ALKENVIRONMENT_H
