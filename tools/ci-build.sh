#!/bin/bash

# SPDX-FileCopyrightText: 2015-2016 Collabora Ltd.
# SPDX-FileCopyrightText: 2020 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: MIT

set -euo pipefail
set -x

# ci_host:
# See ci-install.sh
: "${ci_host:=native}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

# ci_test:
# One of yes,no
: "${ci_test:=yes}"

# start dbus session, which is required by kio
if ! test -v DBUS_SESSION_BUS_PID || test -z "$DBUS_SESSION_BUS_PID"; then
    eval `dbus-launch --sh-syntax`
fi

# enable sudo if running in docker
if [ -f /.dockerenv ] && [ -n `getent passwd | grep user` ]; then
    sudo=sudo
fi

# basic settings
case "$ci_variant" in
    (kf5*)
        cmake_options="-DBUILD_APPLETS=0 -DBUILD_TESTING=1 -DBUILD_WITH_QTNETWORK=1"
        export QT_LOGGING_RULES="*=true"
        start_kde_session=kdeinit5
        ;;

    (kde4)
        cmake_options="-DBUILD_QT4=1 -DKDE4_BUILD_TESTS=1 -DBUILD_WITH_QTNETWORK=1"
        start_kde_session=kdeinit4
        ;;
esac

# custom settings
case "$ci_variant" in
    (kf5)
        cmake_options+=" -DBUILD_WITH_WEBKIT=0 -DBUILD_WITH_WEBENGINE=0"
        ;;
    (kf5-webkit)
        cmake_options+=" -DBUILD_WITH_WEBKIT=1"
        ;;
    (kf5-webengine)
        cmake_options+=" -DBUILD_WITH_WEBENGINE=1"
        ;;
esac

case "$ci_host" in
    (native)
        cmake=cmake
        ;;
    (mingw*)
        cmake="${ci_host}-cmake-kde4 --"
        # not supported yet
        ci_test=no
        ;;
esac

# setup vars
srcdir="$(pwd)"
builddir=ci-build-${ci_variant}-${ci_host}


# create subdirs
rm -rf $builddir
$sudo mkdir -p $builddir
$sudo chmod a+wrx $builddir
cd $builddir

# configure project
$cmake $cmake_options ..
make -j5

if [ ${ci_test} = yes ]; then
    # start xvfb session - will restart in case of crashes
    (while ! test -f ./finished; do xvfb-run -s "+extension GLX +render" -a -n 99 openbox; done) &
    export DISPLAY=:99

    # start kde session
    $start_kde_session

    # run tests
    ctest --output-on-failure --timeout 60
    #ctest -VV --timeout 60

    # show screenshot in case of errors
    if test $? -ne 0; then
        xwd -root -silent | convert xwd:- png:/tmp/screenshot.png
        cat /tmp/screenshot.png | uuencode screenshot
    fi

    # kill x session
    touch finished
    killall -s 9 $start_kde_session openbox dbus-daemon || true
fi

# run install
make install DESTDIR=$PWD/tmp
