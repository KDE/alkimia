#!/bin/bash

# SPDX-FileCopyrightText: 2015-2016 Collabora Ltd.
# SPDX-FileCopyrightText: 2020 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: MIT

set -euo pipefail
set -x

# ci_distro:
# OS distribution in which we are testing
# Typical values: auto opensuse ubuntu
: "${ci_distro:=auto}"

# ci_host:
# the host to build for
: "${ci_host:=native}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

# setup install command; use sudo outside of docker
sudo=
if ! [ -f /.dockerenv ]; then
    sudo=sudo
fi
zypper="$sudo /usr/bin/zypper --non-interactive"

install=
source_install=

if [ "$ci_distro" = "auto" ]; then
    ci_distro=$(. /etc/os-release; echo ${ID})
fi

case "$ci_distro" in
    (opensuse*)
        $zypper modifyrepo --enable repo-source
        # save time
        #$zypper update
        repo=$(. /etc/os-release; echo $PRETTY_NAME | sed 's, ,_,g')
        packages=(
            cmake
            AppStream
        )
        source_packages=()

        # xvfb-run does not have added all required tools
        packages=(
           "${packages[@]}"
            openbox
            psmisc # killall
            shadow # useradd
            sharutils # uuencode
            sudo # sudoers
            xvfb-run
            which
            xauth
            xterm
        )

        # for screenshots
        packages=(
           "${packages[@]}"
            xwd ImageMagick
        )
        case "$ci_variant" in
            (kf5*)
                case "$ci_host" in
                    (native)
                        source_packages=(
                            "${source_packages[@]}"
                            alkimia
                        )
                        packages=(
                            "${packages[@]}"
                            kinit
                        )
                        ;;
                esac
                ;;
            (kde4)
                $zypper ar --refresh --no-gpgcheck \
                    https://download.opensuse.org/repositories/windows:/mingw/$repo/windows:mingw.repo || true

                case "$ci_host" in
                    (native)
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
                    (mingw32|mingw64)
                         # add required repos
                        bits=$(echo $ci_host | sed 's,mingw,,g')
                        prefix=${ci_host}
                        # for mingw packages
                        $zypper ar --refresh --no-gpgcheck \
                           https://download.opensuse.org/repositories/windows:/mingw:/win${bits}/$repo/windows:mingw:win${bits}.repo || true
                        packages=(
                            "${packages[@]}"
                            ${prefix}-cross-gcc-c++
                            ${prefix}-extra-cmake-modules
                            ${prefix}-libkde4-devel
                            ${prefix}-gmp-devel
                        )
                        ;;
                    (*)
                        echo "unsupported value 'ci_host=${ci_host}'"
                        exit 1
                        ;;
                esac
                ;;
        esac

        case "$ci_variant" in
            (kf5-webkit)
                packages=(
                    "${packages[@]}"
                    libQt5WebKitWidgets-devel
                )
                ;;
            (kf5-webengine)
                packages=(
                    "${packages[@]}"
                    libqt5-qtwebengine-devel
                )
                ;;
        esac

        # update package repos
        $zypper refresh
        # install source packages
        if test -v "source_packages"; then
            $zypper source-install "${source_packages[@]}"
        fi
        # install remaining packages
        $zypper install "${packages[@]}"
        ;;
esac

# Add the user that we will use to do the build inside the
# Docker container, and let them use sudo
if [ -f /.dockerenv ] && [ -z `getent passwd | grep user` ]; then
    useradd -m user
    passwd -ud user
    echo "user ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/nopasswd
    chmod 0440 /etc/sudoers.d/nopasswd
fi
