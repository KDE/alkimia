# SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: BSD-3-Clause

# KDE Framework compatibility layer for KDE4
# - provides missing ECM and KDE related macros/functions

set(_kde4_uninstall_rule_created 1)

if(KF4_FIND_REQUIRED)
    set(REQUIRED REQUIRED)
endif()

find_package(KDE4 QUIET ${REQUIRED})
include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})

if(KDE4_FOUND)
    if (BUILD_TESTING)
        set(KDE4_BUILD_TESTS 1)
    endif()
    set(KF4_FOUND 1)
    set(KF4_VERSION ${KDE4_VERSION})

    set(KDE_INSTALL_FULL_AUTH_POLICY_FILES_DIR ${KDE4_AUTH_POLICY_FILES_INSTALL_DIR})
    set(KDE_INSTALL_FULL_AUTOSTART_DIR         ${KDE4_AUTOSTART_INSTALL_DIR})
    set(KDE_INSTALL_FULL_BIN_DIR               ${KDE4_BIN_INSTALL_DIR})
    set(KDE_INSTALL_FULL_CONFIG_DIR            ${KDE4_CONFIG_INSTALL_DIR})
    set(KDE_INSTALL_FULL_DATA_DIR              ${KDE4_DATA_INSTALL_DIR})
    set(KDE_INSTALL_FULL_DBUS_INTERFACES_DIR   ${KDE4_DBUS_INTERFACES_DIR})
    set(KDE_INSTALL_FULL_DBUS_SERVICES_DIR     ${KDE4_DBUS_SERVICES_DIR})
    set(KDE_INSTALL_FULL_DIR                   ${KDE4_INSTALL_DIR})
    set(KDE_INSTALL_FULL_HTML_DIR              ${KDE4_HTML_INSTALL_DIR})
    set(KDE_INSTALL_FULL_ICON_DIR              ${KDE4_ICON_INSTALL_DIR})
    set(KDE_INSTALL_FULL_IMPORTS_DIR           ${KDE4_IMPORTS_INSTALL_DIR})
    set(KDE_INSTALL_FULL_INCLUDE_DIR           ${KDE4_INCLUDE_INSTALL_DIR})
    set(KDE_INSTALL_FULL_INFO_DIR              ${KDE4_INFO_INSTALL_DIR})
    set(KDE_INSTALL_FULL_KCFG_DIR              ${KDE4_KCFG_INSTALL_DIR})
    set(KDE_INSTALL_FULL_KCONF_UPDATE_DIR      ${KDE4_KCONF_UPDATE_INSTALL_DIR})
    set(KDE_INSTALL_FULL_LIBEXEC_DIR           ${KDE4_LIBEXEC_INSTALL_DIR})
    set(KDE_INSTALL_FULL_LIB_DIR               ${KDE4_LIB_INSTALL_DIR})
    set(KDE_INSTALL_FULL_LOCALE_DIR            ${KDE4_LOCALE_INSTALL_DIR})
    set(KDE_INSTALL_FULL_MAN_DIR               ${KDE4_MAN_INSTALL_DIR})
    set(KDE_INSTALL_FULL_METAINFODIR           ${KDE4_DATA_INSTALL_DIR}/metainfo)
    set(KDE_INSTALL_FULL_MIME_DIR              ${KDE4_MIME_INSTALL_DIR})
    set(KDE_INSTALL_FULL_SBIN_DIR              ${KDE4_SBIN_INSTALL_DIR})
    set(KDE_INSTALL_FULL_SERVICES_DIR          ${KDE4_SERVICES_INSTALL_DIR})
    set(KDE_INSTALL_FULL_SERVICETYPES_DIR      ${KDE4_SERVICETYPES_INSTALL_DIR})
    set(KDE_INSTALL_FULL_SOUND_DIR             ${KDE4_SOUND_INSTALL_DIR})
    set(KDE_INSTALL_FULL_SYSCONF_DIR           ${KDE4_SYSCONF_INSTALL_DIR})
    set(KDE_INSTALL_FULL_TEMPLATES_DIR         ${KDE4_TEMPLATES_INSTALL_DIR})
    set(KDE_INSTALL_FULL_WALLPAPER_DIR         ${KDE4_WALLPAPER_INSTALL_DIR})
    set(KDE_INSTALL_FULL_XDG_APPS_DIR          ${KDE4_XDG_APPS_INSTALL_DIR})
    set(KDE_INSTALL_FULL_XDG_DIRECTORY_DIR     ${KDE4_XDG_DIRECTORY_INSTALL_DIR})

    file(RELATIVE_PATH KDE_INSTALL_APPDIR ${KDE4_INSTALL_DIR} ${KDE4_XDG_APPS_INSTALL_DIR})
    file(RELATIVE_PATH KDE_INSTALL_BIN_DIR ${KDE4_INSTALL_DIR} ${KDE4_INCLUDE_INSTALL_DIR})
    file(RELATIVE_PATH KDE_INSTALL_DATA_DIR ${KDE4_INSTALL_DIR} ${KDE4_DATA_INSTALL_DIR})
    file(RELATIVE_PATH KDE_INSTALL_ICONDIR ${KDE4_INSTALL_DIR} ${KDE4_ICON_INSTALL_DIR})
    file(RELATIVE_PATH KDE_INSTALL_INCLUDE_DIR ${KDE4_INSTALL_DIR} ${KDE4_LIB_DIR})
    file(RELATIVE_PATH KDE_INSTALL_LIB_DIR ${KDE4_INSTALL_DIR} ${KDE4_LIB_DIR})
    set(KDE_INSTALL_CMAKEPACKAGEDIR ${KDE_INSTALL_LIB_DIR}/cmake)
    set(KDE_INSTALL_METAINFODIR share/metainfo)

    add_definitions(-DQStringLiteral=QLatin1String)

    set(KF4_COMPONENTS
        kdnssd
        kemoticons
        kfile
        khtml
        kidletime
        kimproxy
        kio
        kjs
        kjsapi
        kjsembed
        kmediaplayer
        knewstuff2
        knewstuff3
        knotifyconfig
        kntlm
        kparts
        kprintutils
        kpty
        krosscore
        krossui
        ksuseinstall
        ktexteditor
        kunitconversion
        kutils
        nepomuk
        nepomukquery
        nepomukutils
        plasma
        solid
        threadweaver
    )

    foreach(COMPONENT ${KF4_COMPONENTS})
        if(TARGET KDE4__${COMPONENT})
            add_library(KF4::${COMPONENT} ALIAS KDE4__${COMPONENT})
        endif()
    endforeach()

# KF >= 5 compatible wrapper
    add_library(KF4::Codecs ALIAS KDE4__kdecore)
    add_library(KF4::Completion ALIAS KDE4__kdecore)
    add_library(KF4::ConfigCore ALIAS KDE4__kdecore)
    add_library(KF4::CoreAddons ALIAS KDE4__kdecore)
    add_library(KF4::Gui ALIAS KDE4__kdeui)
    add_library(KF4::I18n ALIAS KDE4__kdecore)
    add_library(KF4::KIO ALIAS KDE4__kio)
    add_library(KF4::KIOCore ALIAS KDE4__kio)
    add_library(KF4::KIOWidgets ALIAS KDE4__kio)
    add_library(KF4::NewStuff ALIAS KDE4__knewstuff3)
    add_library(KF4::NewStuffWidgets ALIAS KDE4__knewstuff3)
    add_library(KF4::Plasma ALIAS KDE4__plasma)
    add_library(KF4::Solid ALIAS KDE4__solid)
    add_library(KF4::WebKit ALIAS KDE4__kdewebkit)
    add_library(KF4::TextWidgets ALIAS KDE4__kdeui)
    add_library(KF4::IconThemes ALIAS KDE4__kdeui)
    add_library(KF4::Package ALIAS KDE4__kdecore)
    add_library(KF4::WidgetsAddons ALIAS KDE4__kdecore)
    add_library(KF4::XmlGui ALIAS KDE4__kdeui)

    set(KF4_INSTALL_CMAKECONFIG_FILES ${CMAKE_CURRENT_LIST_DIR}/FindKF4.cmake)

    macro(ki18n_wrap_ui)
        kde4_add_ui_files(${ARGN})
    endmacro(ki18n_wrap_ui)

    macro(kconfig_add_kcfg_files)
        kde4_add_kcfg_files(${ARGN})
    endmacro(kconfig_add_kcfg_files)

    macro(ecm_add_executable)
        kde4_add_executable(${ARGN})
    endmacro(ecm_add_executable)

    macro(ecm_mark_nongui_executable)
        foreach(_target ${ARGN})
            set_target_properties(${_target}
                                  PROPERTIES
                                  WIN32_EXECUTABLE FALSE
                                  MACOSX_BUNDLE FALSE
                                 )
        endforeach()
    endmacro(ecm_mark_nongui_executable)

    macro(ecm_install_icons)
        kde4_install_icons(${ICON_INSTALL_DIR})
    endmacro()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KF4 DEFAULT_MSG)

