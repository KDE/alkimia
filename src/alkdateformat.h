/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
 * In 'strict' mode, this is designed to be compatible with the QIF profile date
 * converter.  However, that converter deals with the concept of an apostrophe
 * format in a way I don't understand.  So for the moment, they are 99%
 * compatible, waiting on that issue. (acejones)
 */
class ALK_NO_EXPORT AlkDateFormat
{
public:
    explicit AlkDateFormat(const QString &format);
    ~AlkDateFormat();

    AlkDateFormat& operator=(const AlkDateFormat&);

    typedef enum {
      NoError = 0,
      InvalidFormatString,
      InvalidFormatCharacter,
      InvalidDate,
      InvalidDay,
      InvalidMonth,
      InvalidYear,
      InvalidYearLength,
    } ErrorCode;

    QString convertDate(const QDate &date);
    QDate convertString(const QString &date, bool strict = true,
                              unsigned centuryMidPoint = QDate::currentDate().year());
    const QString &format() const;

    QString lastErrorMessage() const;
    ErrorCode lastError() const;

private:
    class Private;
    Private* const d;
};

#endif // ALKDATEFORMAT_H
