# SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: BSD-3-Clause

# replacement for the same file from ECM package with KF4 support

if (QT_MAJOR_VERSION EQUAL 4)
    set(KDE_SKIP_RPATH_SETTINGS 1)
else()
    include(${ECM_KDE_MODULE_DIR}/KDEInstallDirs${QT_MAJOR_VERSION}.cmake)
endif()
