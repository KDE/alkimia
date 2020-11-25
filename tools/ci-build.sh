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

# ci_host:
# See ci-install.sh
: "${ci_host:=native}"

# ci_variant:
# One of kf5, kde4
: "${ci_variant:=kf5}"

# start dbus session, which is required by kio
if ! test -v DBUS_SESSION_BUS_PID || test -z "$DBUS_SESSION_BUS_PID"; then
    eval `dbus-launch --sh-syntax`
fi

case "$ci_variant" in
    (kf5)
        cmake_options="-DBUILD_APPLETS=0 -DBUILD_TESTING=1"
        export QT_LOGGING_RULES="*=true"
        start_kde_session=kdeinit5
        ;;

    (kde4)
        cmake_options="-DBUILD_QT4=1 -DKDE4_BUILD_TESTS=1"
        start_kde_session=kdeinit4
        ;;
esac

# create subdirs
srcdir="$(pwd)"
rm -rf ci-build-${ci_variant}-${ci_host}
mkdir -p ci-build-${ci_variant}-${ci_host}
cd ci-build-${ci_variant}-${ci_host}

# configure project
cmake $cmake_options ..
make -j5

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

# run install
make install DESTDIR=$PWD/tmp

# kill x session
touch finished
killall -s 9 $start_kde_session openbox dbus-daemon || true
