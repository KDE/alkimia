/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <alkimia/alkversion.h>

QString alkVersionString()
{
    return ALK_VERSION_STRING;
}

unsigned int alkVersion()
{
    return ALK_VERSION;
}
