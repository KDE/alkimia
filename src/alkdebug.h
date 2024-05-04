/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

/**
 * @file
 * @brief Wrapper for debug output
 */

#ifndef ALKDEBUG_H
#define ALKDEBUG_H

#include <QtDebug>

// we do not want to support qDebug
#undef qDebug
QDebug _alkDebug(const char *file, int line, const char *func);
#define alkDebug() _alkDebug(__FILE__, __LINE__, Q_FUNC_INFO)

#endif // ALKDEBUG_H
