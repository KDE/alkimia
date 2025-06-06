#!/bin/bash

# SPDX-FileCopyrightText: 2015-2016 Collabora Ltd.
# SPDX-FileCopyrightText: 2020-2024 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: MIT

# add timestamps
export PS4='[$(date "+%T.%3N")]'" $PS4"

set -euo pipefail
set -x

# kill kde and x session
function cleanup() {
    touch $builddir/finished
    stop_session
    stop_webserver
    if [ "$ci_in_docker" = yes ]; then
        cleanup_docker
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

function stop_x_session() {
    killall -s 9 xvfb-run
    sleep 1
    killall -s 9 Xvfb
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

function stop_kde_session() {
    if test "$ci_host" = native; then
        ${start_kde_session}_shutdown || true
        if [ -v DBUS_SESSION_BUS_PID ]; then
            kill -s 9 $DBUS_SESSION_BUS_PID
        fi
    else
        $wrapper $start_kde_session --terminate
    fi
}

function start_session() {
    if [[ "$ci_variant" =~ "^kf5.*" ]] && test -v dep_prefix; then
        # setup qt5.conf
        qtconf="$dep_prefix/bin/qt5.conf"
        sed "s,Prefix.*$,Prefix=$dep_prefix,g" "$dep_prefix/bin/qt5.conf" > "$builddir/bin/qt5.conf"
    fi

    if [[ "$ci_variant" =~ "^kf6.*" ]] && test -v dep_prefix; then
        # setup qt6.conf
        qtconf="$dep_prefix/bin/qt6.conf"
        sed "s,Prefix.*$,Prefix=$dep_prefix,g" "$dep_prefix/bin/qt6.conf" > "$builddir/bin/qt6.conf"
    fi

    if test "$ci_in_docker" = yes; then
        start_x_session
        start_kde_session
    fi
}

function stop_session() {
    if test "$ci_in_docker" = yes; then
        stop_kde_session
        stop_x_session
    fi
}

# start webserver for testing
function start_webserver() {
    if [ "$ci_webserver" = yes ]; then
        n=$(gawk '$2 ~ TEST_HOST { print $2 }' TEST_HOST=$test_host /etc/hosts)
        if test -z "$n"; then
            echo "127.0.0.1 $test_host" | $sudo tee --append /etc/hosts
        fi
        $sudo php8 -S "$test_host:80" -t $srcdir/tools &
    fi
}

# stop webserver for testing
function stop_webserver() {
    if [ "$ci_webserver" = yes ]; then
        pid=$(ps aux | grep php8 | grep "$test_host" | gawk '{ print $2 }')
        if test -n "$pid"; then
            $sudo kill -s 9 $pid
        fi
    fi
}

function init_docker() {
    mkdir -p ~/.config/gdb
    cat << EOF > ~/.config/gdb/gdbinit
print("loading gdb event handler")
python

# for pretty printer
gdb.execute("set auto-load safe-path /")

def stop_handler (event):
    print ("event type: stop")
    gdb.execute("set pagination off")
    gdb.execute("thread apply all bt")
    gdb.execute("set confirm off")
    gdb.execute("quit 1")

gdb.events.stop.connect (stop_handler)
end
EOF

}

function cleanup_docker() {
    rm ~/.config/gdb/gdbinit
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

# missing wrapper for associated rpm macro
function cmake-kf6() {
    NAME="cmake_kf6"
    ${RPM_OPT_FLAGS:=}
    ${LDFLAGS:=}
    ${icerun:=}
    #eval "`rpm --eval "%${NAME} $(printf " %q" "${@}")"`"
    $(echo "${@}" | sed 's,--,cmake,g')
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

# ci_parallel:
# A number of parallel jobs, passed to make -j
: "${ci_parallel:=1}"

# ci_in_docker:
# flags to indicate that we are running in docker
: "${ci_in_docker:=auto}"

# ci_runtime:
# One of static, shared; used for windows cross builds
: "${ci_runtime:=shared}"

# ci_test:
# If yes, run tests; if no, just build
: "${ci_test:=yes}"

# ci_variant:
# One of kf6, kf5, kf4
: "${ci_variant:=kf5}"

# ci_webserver:
# if yes, run simple webserver for testing
: "${ci_webserver:=yes}"

# ci_cmake_options:
# empty if not set
: "${ci_cmake_options:=}"

# print used command line
set +x; env | awk 'BEGIN { s = "" } $1 ~ /^ci_/ { s=s " " $0} END { print s " " SCRIPT }' SCRIPT=$0; set -x

# host name for testing
test_host=dev.kmymoney.org

# specify build dir
srcdir="$(pwd)"
builddir=${srcdir}/ci-build-${ci_variant}-${ci_host}

# check and setup if running in docker
# found on https://stackoverflow.com/questions/23513045
if [ $ci_in_docker = auto ]; then
    case $(cat /proc/1/sched  | head -n 1 | cut -d' ' -f1) in
    systemd|init)
        ci_in_docker=no
        ;;
    *)
        ci_in_docker=yes
        ;;
    esac
fi

# enable sudo if running in docker
sudo=
if [ "$ci_in_docker" = "yes" ] && [ -n `getent passwd | grep ^user` ]; then
    sudo=sudo
fi

# common cmake options
cmake_options=$ci_cmake_options

# check if webserver is enabled
if [ "$ci_webserver" = yes ]; then
    # todo add separate host to avoid conflicts host
    cmake_options+=" -DTEST_DEVELOP_HOST=1"
fi

# check if running in docker
if [ "$ci_in_docker" = yes ]; then
    cmake_options+=" -DTEST_IN_DOCKER=1 -DCMAKE_TEST_LAUNCHER=/usr/bin/gdb;-q;-ex;r;-ex;q"
    init_docker
fi

# check if running in gitlab CI
if [[ -v CI_RUNNER_VERSION ]]; then
    cmake_options+=" -DTEST_IN_GITLAB_CI=1"
fi

# settings for build variants
case "$ci_variant" in
    (kf6*)
        cmake_options+=" -DBUILD_APPLETS=0 -DBUILD_TESTING=1 -DENABLE_CLIENT_PACKAGE_TEST=1 -DBUILD_WITH_QT6=1"
        cmake_suffix="kf6"
        export QT_LOGGING_RULES="*=true;kf.kio.workers.http.debug=false;qt.text.*.debug=false"
        export QT_FORCE_STDERR_LOGGING=1
        export QT_ASSUME_STDERR_HAS_CONSOLE=1
        start_kde_session=kdeinit5
        ;;

    (kf5*)
        cmake_options+=" -DBUILD_APPLETS=0 -DBUILD_TESTING=1 -DENABLE_CLIENT_PACKAGE_TEST=1"
        cmake_suffix="kf5"
        export QT_LOGGING_RULES="*=true;kf.kio.workers.http.debug=false;qt.text.*.debug=false"
        export QT_FORCE_STDERR_LOGGING=1
        export QT_ASSUME_STDERR_HAS_CONSOLE=1
        start_kde_session=kdeinit5
        ;;

    (kf4)
        cmake_options+=" -DBUILD_QT4=1 -DBUILD_TESTING=1"
        cmake_suffix="kde4"
        start_kde_session=kdeinit4
        ;;
esac

# for building
cmake=cmake

# settings for platforms
case "$ci_host" in
    (mingw32)
        cmake_options+=" -DQT_MOC_EXECUTABLE=/usr/i686-w64-mingw32/bin/moc"
        cmake_configure="$ci_host-cmake-$cmake_suffix"
        init_cross_runtime i686-w64-mingw32 $builddir/bin
        wrapper=/usr/bin/wine
        ;;
    (mingw64)
        cmake_options+=" -DAUTOMOC_EXECUTABLE=/usr/bin/x86_64-w64-mingw32-moc"
        cmake_configure="$ci_host-cmake-$cmake_suffix"
        init_cross_runtime x86_64-w64-mingw32 $builddir/bin
        wrapper=/usr/bin/wine
        ;;
    (*)
        cmake_options+=" -DCMAKE_CXX_FLAGS=-fPIC"
        cmake_configure="cmake-$cmake_suffix"
        export LD_LIBRARY_PATH=${builddir}/bin
        wrapper=
        ;;
esac

# custom settings
cmake_options+=""
case "$ci_variant" in
    (kf[56])
        cmake_options+=" -DBUILD_WITH_WEBKIT=0 -DBUILD_WITH_WEBENGINE=0"
        ;;
    (kf*-webkit)
        cmake_options+=" -DBUILD_WITH_WEBKIT=1"
        ;;
    (kf*-webengine)
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
    $cmake --build ${builddir} -j$ci_parallel
fi

# run tests
echo "checking for running tests = $ci_test"
if test "$ci_test" = yes; then
    trap cleanup EXIT

    start_session

    start_webserver

    # run tests
    ctest --test-dir ${builddir} --timeout 60 --parallel $ci_parallel -VV

    # show screenshot in case of errors
    if test $? -ne 0; then
        xwd -root -silent | convert xwd:- png:/tmp/screenshot.png
        cat /tmp/screenshot.png | uuencode screenshot
    fi
fi

# run install
$cmake --build ${builddir} -t install DESTDIR=$PWD/tmp
