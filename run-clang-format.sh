#!/bin/sh

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2026 Ralf Habacker <ralf.habacker@freenet.de>

minversion=15
version=$(clang-format --version | sed "s,^.*\s,,g;+s,\..*$,,g")

if test "$version" -lt "$minversion"; then
    echo "clang-format version $version found, but version >= $minversion required"
    exit
fi

find autotests examples plasma qml src tests tools \( -name "*.cpp" -or -name "*.h"  -or -name "*.c" \) -exec clang-format -i {} \;
