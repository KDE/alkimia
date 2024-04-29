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

// alkwebpagetest.cpp, alkonlinequotesource.cpp
#define TEST_LAUNCH_URL TEST_DOWNLOAD_HOST "/onlinequotestest.php?"
// alkdownloadenginetest.cpp
#define TEST_DOWNLOAD_URL_PRICE TEST_LAUNCH_URL "a=EUR"
#define TEST_DOWNLOAD_URL_CURRENCY TEST_LAUNCH_URL "a=BTC&b=GBP"
#define TEST_DOWNLOAD_URL_ERROR TEST_LAUNCH_URL "servererror=1"

#endif // ALKTESTDEFS_H
