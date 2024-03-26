/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKTESTDEFS_H
#define ALKTESTDEFS_H

#ifndef TEST_DEVELOP_HOST
#define TEST_DOWNLOAD_HOST "https://kmymoney.org"
#else
#define TEST_DOWNLOAD_HOST "http://dev.kmymoney.org"
#endif

// alkwebpagetest.cpp
#define TEST_LAUNCH_URL TEST_DOWNLOAD_HOST "/onlinequotestest.php?"

#endif // ALKTESTDEFS_H
