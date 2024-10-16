#!/bin/bash

# SPDX-FileCopyrightText: 2015-2016 Collabora Ltd.
# SPDX-FileCopyrightText: 2020-2024 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: MIT

set -euo pipefail
set -x

# ci_distro:
# OS distribution in which we are testing
# Typical values: auto opensuse ubuntu
: "${ci_distro:=auto}"

# ci_host:
# the host to build for native, mingw32, mingw64
: "${ci_host:=native}"

# ci_distro_variant:
# Typical values: leap tumbleweed
: "${ci_distro_variant:=leap}"

# ci_variant:
# One of kf5, kf4
: "${ci_variant:=kf5}"

# ci_webserver:
# if yes, install simple webserver
: "${ci_webserver:=yes}"

# setup install command; use sudo outside of docker
# found on https://stackoverflow.com/questions/23513045
case $(cat /proc/1/sched  | head -n 1 | cut -d' ' -f1) in
  systemd|init)
    sudo=sudo
    ci_in_docker="no"
    ;;
  *)
    ci_in_docker="yes"
    sudo=
    ;;
esac

zypper="$sudo /usr/bin/zypper --non-interactive"

install=
source_install=

if [ "$ci_distro" = "auto" ]; then
    ci_distro=$(. /etc/os-release; echo ${ID})
fi

case "$ci_distro" in
    (opensuse*)
        # add required repos
        repo_name=$(. /etc/os-release; echo $PRETTY_NAME | sed 's, ,_,g')
        repos=()
        # install newest cmake
        case "$repo_name" in
            (*Leap*)
                devel_tools_building_repo_name=$(. /etc/os-release; echo $VERSION_ID)
                ;;
            (*Tumbleweed*)
                devel_tools_building_repo_name=openSUSE_Factory
                ;;
        esac
        repos=(
            "${repos[@]}"
            https://download.opensuse.org/repositories/devel:/tools:/building/$devel_tools_building_repo_name/devel:tools:building.repo
        )
        case "$ci_variant-$ci_host" in
            (*-mingw*)
                bits=$(echo $ci_host | sed 's,mingw,,g')
                repos=(
                    "${repos[@]}"
                    https://download.opensuse.org/repositories/windows:/mingw:/win${bits}/${repo_name}/windows:mingw:win${bits}.repo
                    https://download.opensuse.org/repositories/windows:/mingw/${repo_name}/windows:mingw.repo
                    https://download.opensuse.org/repositories/security:/tls/openSUSE_Tumbleweed/security:tls.repo
                )
                ;;
            (kf6-native)
                repos=(
                    "${repos[@]}"
                    https://download.opensuse.org/repositories/KDE:/Unstable:/Frameworks/openSUSE_Factory/KDE:Unstable:Frameworks.repo
                )
                ;;

            (kf4-native)
                repos=(
                    "${repos[@]}"
                    https://download.opensuse.org/repositories/windows:/mingw/${repo_name}/windows:mingw.repo
                )
                ;;
        esac

        # setup packages
        packages=(
            cmake
            AppStream
        )
        source_packages=()

        # misc packages
        packages=(
           "${packages[@]}"
            gawk
            gettext-runtime
            # for catching segfaults
            gdb
            # prevents crashing of mingwxx-windres (https://bugzilla.opensuse.org/show_bug.cgi?id=1198923)
            glibc-locale-base
            # xvfb-run does not have added all required tools
            openbox
            procps # ps
            psmisc # killall
            shadow # useradd
            sharutils # uuencode
            sudo # sudoers
            xvfb-run
            which
            xauth
            xterm
        )

        if [ "$ci_webserver" = "yes" ]; then
            packages=(
            "${packages[@]}"
                php8-cli # webserver
            )
        fi

        # for screenshots
        packages=(
           "${packages[@]}"
            xwd ImageMagick
        )
        case "$ci_variant-$ci_host" in
            (kf6*-native)
                source_packages=(
                    "${source_packages[@]}"
                )
                packages=(
                    "${packages[@]}"
                    kinit
                    "cmake(KF6Completion)"
                    "cmake(KF6Config)"
                    "cmake(KF6CoreAddons)"
                    "cmake(KF6I18n)"
                    "cmake(KF6IconThemes)"
                    "cmake(KF6KIO)"
                    "cmake(KF6NewStuff)"
                    "cmake(KF6NewStuffCore)"
                    "cmake(KF6Package)"
                    "cmake(KF6TextWidgets)"
                    "cmake(KF6WidgetsAddons)"
                    "cmake(Qt6Core)"
                    "cmake(Qt6DBus)"
                    "cmake(Qt6Qml)"
                    "cmake(Qt6Test)"
                    "cmake(Qt6Widgets)"
                    "doxygen"
                    "gmp-devel"
                    "kf6-extra-cmake-modules"
                    "qt6qmlimport(org.kde.newstuff)"
                )
                ;;
            (kf5*-native)
                source_packages=(
                    "${source_packages[@]}"
                    alkimia
                )
                packages=(
                    "${packages[@]}"
                    kinit
                )
                ;;
            (kf5*-mingw*)
                prefix=${ci_host}
                packages=(
                    "${packages[@]}"
                    doxygen
                    "$prefix-extra-cmake-modules"
                    "$prefix-gmp-devel"
                    "$prefix(cmake:KF5Completion)"
                    "$prefix(cmake:KF5Config)"
                    "$prefix(cmake:KF5CoreAddons)"
                    "$prefix(cmake:KF5I18n)"
                    "$prefix(cmake:KF5IconThemes)"
                    "$prefix(cmake:KF5KIO)"
                    "$prefix(cmake:KF5NewStuff)"
                    "$prefix(cmake:KF5Package)"
                    "$prefix(cmake:KF5TextWidgets)"
                    "$prefix(cmake:Qt5Core)"
                    "$prefix(cmake:Qt5DBus)"
                    "$prefix(cmake:Qt5Qml)"
                    "$prefix(cmake:Qt5Test)"
                    "$prefix(cmake:Qt5WebKit)"
                    "$prefix(cmake:Qt5Widgets)"
                    wine
                )
                ;;
            (kf4-native)
                # for libQtWebKit-devel
                packages=(
                    "${packages[@]}"
                    gcc-c++
                    extra-cmake-modules
                    libkde4-devel
                    libQtWebKit-devel
                    kdebase4-runtime
                    gmp-devel
                )
                ;;
            (kf4-mingw*)
                prefix=${ci_host}
                packages=(
                    "${packages[@]}"
                    ${prefix}-cross-gcc-c++
                    ${prefix}-extra-cmake-modules
                    ${prefix}-libkf4-devel
                    ${prefix}-gmp-devel
                    wine
                )
                ;;
            (*)
                echo "unsupported combination '$ci_variant=${ci_variant} ci_host=${ci_host}'"
                exit 1
                ;;
        esac

        case "$ci_variant" in
            (kf5-webkit)
                packages=(
                    "${packages[@]}"
                    "cmake(Qt5WebKitWidgets)"
                )
                ;;
            (kf5-webengine)
                packages=(
                    "${packages[@]}"
                    "google-droid-fonts"
                    "cmake(Qt5WebEngineWidgets)"
                )
                ;;
            (kf6-webengine)
                packages=(
                    "${packages[@]}"
                    "google-droid-fonts"
                    "cmake(Qt6WebEngineWidgets)"
                )
                ;;
        esac

        # add repos
        for r in ${repos[@]}; do
            $zypper ar --no-refresh --no-check --no-gpgcheck $r || true
        done

        if test -v "source_packages"; then
            # enable source repo
            $zypper modifyrepo --enable repo-source
            # install source dependencies
            $zypper source-install -d "${source_packages[@]}"
        fi

        # install remaining packages
        $zypper install "${packages[@]}"
        ;;
esac

# Add the user that we will use to do the build inside the
# Docker container, and let them use sudo
if [ "$ci_in_docker" = "yes" ] && [ -z `getent passwd | grep ^user` ]; then
    useradd -m user
    passwd -ud user
    echo "user ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/nopasswd
    chmod 0440 /etc/sudoers.d/nopasswd
fi
