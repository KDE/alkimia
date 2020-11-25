#!/bin/bash

# Copyright © 2015-2016 Collabora Ltd.
# Copyright © 2020 Ralf Habacker <ralf.habacker@freenet.de>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

set -euo pipefail
set -x

# ci_distro:
# OS distribution in which we are testing
# Typical values: opensuse ubuntu
: "${ci_distro:=opensuse}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

zypper="/usr/bin/zypper --non-interactive"

install=
source_install=

case "$ci_distro" in
    (opensuse)
        $zypper modifyrepo --enable repo-source
        # save time
        #$zypper update
        repo=$(. /etc/os-release; echo $PRETTY_NAME | sed 's, ,_,g')
        packages=(cmake)
        source_packages=()

        # xvfb-run does not have added all required tools
        packages=(
           "${packages[@]}"
            openbox
            psmisc # killall
            sharutils # uuencode
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
            (kf5)
                source_packages=(
                    "${source_packages[@]}"
                    alkimia
                )
                packages=(
                    "${packages[@]}"
                    kinit
                    libQt5WebKitWidgets-devel
                )
                ;;

            (kde4)
                # for libQtWebKit-devel
                $zypper ar --refresh --no-gpgcheck \
                    https://download.opensuse.org/repositories/windows:/mingw/$repo/windows:mingw.repo || true
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
