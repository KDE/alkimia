#!/bin/bash

# SPDX-FileCopyrightText: 2015-2016 Collabora Ltd.
# SPDX-FileCopyrightText: 2020,2022,2023 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: MIT

set -euo pipefail
set -x

# kill kde and x session
function cleanup() {
    touch finished
    if test "$ci_in_docker" = yes; then
        if test "$ci_host" = native; then
            ${start_kde_session}_shutdown
            kill -s 9 $DBUS_SESSION_BUS_PID
        else
            $wrapper $start_kde_session --terminate
        fi
        killall -s 9 xvfb-run
        sleep 1
        killall -s 9 Xvfb
    fi
}

# start xvfb session - will restart in case of crashes
function start_x_session() {
    rm -f $builddir/finished
    (
        while ! test -f $builddir/finished; do
            xvfb-run -s "+extension GLX +render" -a -n 99 openbox 2>&1 >/dev/null
        done
    ) &
    export DISPLAY=:99
    sleep 2
}

# start dbus-daemon and kde background processes
function start_kde_session() {
    if test "$ci_host" = native; then
        # start new dbus session, which is required by kio
        # and identified by $DBUS_SESSION_BUS_PID
        eval `dbus-launch --sh-syntax`
    fi
    $wrapper $start_kde_session --verbose
}

function start_session() {
    if test "$ci_variant" = kf5 && test -v dep_prefix; then
        # setup qt5.conf
        qtconf="$dep_prefix/bin/qt5.conf"
        sed "s,Prefix.*$,Prefix=$dep_prefix,g" "$dep_prefix/bin/qt5.conf" > "$builddir/bin/qt5.conf"
    fi

    if test "$ci_in_docker" = yes; then
        start_x_session
        start_kde_session
    fi
}

# missing wrapper for associated rpm macro
function cmake-kde4() {
    NAME="cmake_kde4"
    ${RPM_OPT_FLAGS:=}
    ${LDFLAGS:=}
    ${icerun:=}
    eval "`rpm --eval "%${NAME} $(printf " %q" "${@}")"`"
}

# missing wrapper for associated rpm macro
function cmake-kf5() {
    NAME="cmake_kf5"
    ${RPM_OPT_FLAGS:=}
    ${LDFLAGS:=}
    ${icerun:=}
    eval "`rpm --eval "%${NAME} $(printf " %q" "${@}")"`"
}

##
## initialize support to run cross compiled executables
##
# syntax: init_wine <path1> [<path2> ... [<pathn>]]
# @param  path1..n  pathes for adding to wine executable search path
#
# The function exits the shell script in case of errors
#
init_wine() {
    if ! command -v wineboot >/dev/null; then
        echo "wineboot not found"
        exit 1
    fi

    # run without X11 display to avoid that wineboot shows dialogs
    wineboot -fi

    # add local paths to wine user path
    local addpath="" d="" i
    for i in "$@"; do
        local wb=$(winepath -w "$i")
        addpath="$addpath$d$wb"
        d=";"
    done

    # create registry file from template
    local wineaddpath=$(echo "$addpath" | sed 's,\\,\\\\\\\\,g')
    r=$(realpath $0)
    r=$(dirname $r)
    sed "s,@PATH@,$wineaddpath,g" "$r/user-path.reg.in" > user-path.reg

    # add path to registry
    wine regedit /C user-path.reg

    # check if path(s) has been set and break if not
    local o=$(wine cmd /C "echo %PATH%")
    case "$o" in
        (*z:* | *Z:*)
            # OK
            ;;
        (*)
            echo "Failed to add Unix paths '$*' to path: Wine %PATH% = $o" >&2
            exit 1
            ;;
    esac
}

#
# prepare running cross compile executables
#
# @param prefix   host prefix to build for e.g. i686-w64-mingw32
# @param binpath  path for executables in build root
#
init_cross_runtime() {
    local _prefix="$1"
    local binpath="$2"
    # CFLAGS and CXXFLAGS does do work, checked with cmake 3.15
    export LDFLAGS="-${ci_runtime}-libgcc"
    # enable tests if supported
    if [ "$ci_test" = yes ]; then
        sysroot=$("${_prefix}-gcc" --print-sysroot)
        # check if the prefix is a subdir of sysroot (e.g. openSUSE)
        if [ -d "${sysroot}/${_prefix}" ]; then
            dep_prefix="${sysroot}/${_prefix}"
        else
            # fallback: assume the dependency libraries were built with --prefix=/${ci_host}
            dep_prefix="/${_prefix}"
        fi
        # choose correct wine architecture
        if [ "${_prefix%%-*}" = x86_64 ]; then
            export WINEARCH=win64
            export WINEPREFIX=${HOME}/.wine64
        else
            export WINEARCH=win32
            export WINEPREFIX=${HOME}/.wine32
        fi
        export WINEDEBUG=fixme-all
        # clean wine prefix
        rm -rf ${WINEPREFIX}
        libgcc_path=
        if [ "$ci_runtime" = "shared" ]; then
            libgcc_path=$(dirname "$("${_prefix}-gcc" -print-libgcc-file-name)")
        fi

        init_wine "${dep_prefix}/bin" "$2" ${libgcc_path:+"$libgcc_path"}
    fi
}

# ci_build:
# used for debugging
: "${ci_build:=yes}"

# ci_clean:
# used for debugging
: "${ci_clean:=yes}"

# ci_host:
# See ci-install.sh
: "${ci_host:=native}"

# ci_in_docker:
# flags to indicate that we are running in docker
: "${ci_in_docker:=yes}"

# ci_jobs:
# number of jobs
: "${ci_jobs:=5}"

# ci_runtime:
# One of static, shared; used for windows cross builds
: "${ci_runtime:=shared}"

# ci_test:
# If yes, run tests; if no, just build
: "${ci_test:=yes}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

# specify build dir
srcdir="$(pwd)"
builddir=${srcdir}/ci-build-${ci_variant}-${ci_host}


# enable sudo if running in docker
sudo=
if [ -f /.dockerenv ] && [ -n `getent passwd | grep ^user` ]; then
    sudo=sudo
fi

# settings for build variants
case "$ci_variant" in
    (kf5*)
        cmake_options="-DBUILD_APPLETS=0 -DBUILD_TESTING=1 -DBUILD_WITH_QTNETWORK=1"
        cmake_suffix="kf5"
        export QT_LOGGING_RULES="*=true"
        export QT_FORCE_STDERR_LOGGING=1
        export QT_ASSUME_STDERR_HAS_CONSOLE=1
        start_kde_session=kdeinit5
        ;;

    (kde4)
        cmake_options="-DBUILD_QT4=1 -DKDE4_BUILD_TESTS=1 -DBUILD_WITH_QTNETWORK=1"
        cmake_suffix="kde4"
        start_kde_session=kdeinit4
        ;;
esac

# for building
cmake=cmake

# settings for platforms
case "$ci_host" in
    (mingw32)
        # not yet supported
        ci_test = no
        cmake_configure="$ci_host-cmake-$cmake_suffix"
        init_cross_runtime i686-w64-mingw32 $builddir/bin
        wrapper=/usr/bin/wine
        ;;
    (mingw64)
        # not yet supported
        ci_test = no
        cmake_configure="$ci_host-cmake-$cmake_suffix"
        init_cross_runtime x86_64-w64-mingw32 $builddir/bin
        wrapper=/usr/bin/wine
        ;;
    (*)
        cmake_configure="cmake-$cmake_suffix"
        export LD_LIBRARY_PATH=${builddir}/bin
        wrapper=
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

# setup vars
srcdir="$(pwd)"
builddir=${srcdir}/ci-build-${ci_variant}-${ci_host}


# create subdirs
if test "$ci_clean" = yes; then
    rm -rf ${builddir}
    $sudo mkdir -p ${builddir}
    $sudo chmod a+wrx ${builddir}
fi

# configure and build
if test "$ci_build" = yes; then
    $cmake_configure -- -S ${srcdir} -B ${builddir} $cmake_options
    $cmake --build ${builddir} -j$ci_jobs
fi

# run tests
echo "checking for running tests = $ci_test"
if test "$ci_test" = yes; then
    trap cleanup EXIT

    start_session

    # run tests
    ctest --test-dir ${builddir} --output-on-failure --timeout 60 --jobs $ci_jobs

    # show screenshot in case of errors
    if test $? -ne 0; then
        xwd -root -silent | convert xwd:- png:/tmp/screenshot.png
        cat /tmp/screenshot.png | uuencode screenshot
    fi
fi

# run install
$cmake --build ${builddir} -t install DESTDIR=$PWD/tmp
