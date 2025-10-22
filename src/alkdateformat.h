/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKDATEFORMAT_H
#define ALKDATEFORMAT_H

#include "alkexception.h"

#include <QDate>
#include <QString>
#include <QStringList>

/**
 *
 * Universal date converter
 */
class ALK_NO_EXPORT AlkDateFormat
{
public:
    /**
     * A constructor to create an object for converting strings to date
     *
     * @param format format string for converting a string into a date
     */
    explicit AlkDateFormat(const QString &format);
    AlkDateFormat(const AlkDateFormat& right);
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

    /**
     * Converts a string into a date by using a specific format string
     *
     * Supported format strings:
     *
     * - \%d  - day with or without leading zeros
     * - \%m  - month with or without leading zeros
     * - \%u  - time since 1970-01-01 in seconds
     * - \%ud - days since 1970-01-01
     * - \%um - time since 1970-01-01 in milliseconds
     * - \%y - year with 2 or 4 digits. In the case of 2 digits, a year within +/- 50 years
     *   of the \p centuryMidPoint parameter is selected, i.e. if the midpoint is 2000,
     *   0-49 becomes 2000-2049 and 50-99 becomes 1950-1999.
     * - UNIX - time since 1970-01-01 in seconds
     *
     * @param date the string to convert by using the format string which is specified using a constructor of this class
     * @param strict if true, the separators specified in the format are enforced. Otherwise, any separators are allowed
     * In 'strict' mode, this is designed to be compatible with the QIF profile date
     * converter.
     * @param centuryMidPoint century midpoint for 2-digit years
     * @return string, converted to a date or an invalid date in case of errors,
     *         lastError() returns the cause of the error
     */
    QDate convertString(const QString &date, bool strict = true,
                              unsigned centuryMidPoint = QDate::currentDate().year());

    /**
     * Return format string used by convertString()
     * @return format string
     */
    const QString &format() const;

    /**
     * Return string matching the result from lastError()
     * @return string with error message
     */
    QString lastErrorMessage() const;

    /**
     * Return error code in case convertString() fails to convert the string into a date
     * @return error code
     */
    ErrorCode lastError() const;

private:
    class Private;
    Private* const d;
};

#endif // ALKDATEFORMAT_H
