/*
    SPDX-FileCopyrightText: 2000-2018 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkexception.h"

AlkException::AlkException(const QString &msg, const QString &file, const unsigned long line)
{
    // qDebug("ALKEXCEPTION(%s,%s,%ul)", qPrintable(msg), qPrintable(file), line);
    m_msg = msg;
    m_file = file;
    m_line = line;
}

AlkException::~AlkException()
{
}
