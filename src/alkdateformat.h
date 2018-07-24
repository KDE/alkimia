/***************************************************************************
 *   Copyright 2004  Ace Jones <acejones@users.sourceforge.net>            *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#ifndef ALKDATEFORMAT_H
#define ALKDATEFORMAT_H

#include "alkexception.h"

#include <QDate>
#include <QRegExp>
#include <QString>
#include <QStringList>

/**
 *
 * Universal date converter
 *
 * In 'strict' mode, this is designed to be compatable with the QIF profile date
 * converter.  However, that converter deals with the concept of an apostrophe
 * format in a way I don't understand.  So for the moment, they are 99%
 * compatable, waiting on that issue. (acejones)
 */
class ALK_NO_EXPORT AlkDateFormat
{
public:
    explicit AlkDateFormat(const QString &_format) : m_format(_format)
    {
    }

    const QString convertDate(const QDate &_in) const;
    const QDate convertString(const QString &_in, bool _strict = true,
                              unsigned _centurymidpoint = QDate::currentDate().year()) const;
    const QString &format() const
    {
        return m_format;
    }

private:
    QString m_format;
};

#endif // ALKDATEFORMAT_H
