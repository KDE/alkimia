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

#include "alkdateformat.h"

#include <QtDebug>

const QDate AlkDateFormat::convertString(const QString &_in, bool _strict,
                                         unsigned _centurymidpoint) const
{
    if (m_format.contains("%"))
        return convertStringKMyMoney(_in, _strict, _centurymidpoint);
    else
        return convertStringSkrooge(_in);
}

const QDate AlkDateFormat::convertStringSkrooge(const QString &_in) const
{
    QDate date;
    if (m_format == "UNIX") {
        date = QDateTime::fromTime_t((_in.toUInt())).date();
    } else {
        date = QDate::fromString(_in, m_format);
        // Try with an english locale
        if (!date.isValid()) {
            QLocale en("en_EN");
            date = en.toDate(_in, m_format);
        }
    }
    if (!date.isValid()) {
        throw ALKEXCEPTION(QString("Invalid date '%1'").arg(_in));
    }
    if (!m_format.contains(QStringLiteral("yyyy")) && date.year() < 2000)
        date = date.addYears(100);
    return date;
}

#if QT_VERSION < 0x050000
#include <KGlobal>
#include <KCalendarSystem>

const QDate AlkDateFormat::convertStringKMyMoney(const QString &_in, bool _strict,
                                                 unsigned _centurymidpoint) const
{
    //
    // Break date format string into component parts
    //

    QRegExp formatrex("%([mdy]+)(\\W+)%([mdy]+)(\\W+)%([mdy]+)", Qt::CaseInsensitive);
    if (formatrex.indexIn(m_format) == -1) {
        throw ALKEXCEPTION("Invalid format string");
    }

    QStringList formatParts;
    formatParts += formatrex.cap(1);
    formatParts += formatrex.cap(3);
    formatParts += formatrex.cap(5);

    QStringList formatDelimiters;
    formatDelimiters += formatrex.cap(2);
    formatDelimiters += formatrex.cap(4);

    //
    // Break input string up into component parts,
    // using the delimiters found in the format string
    //

    QRegExp inputrex;
    inputrex.setCaseSensitivity(Qt::CaseInsensitive);

    // strict mode means we must enforce the delimiters as specified in the
    // format.  non-strict allows any delimiters
    if (_strict) {
        inputrex.setPattern(QString("(\\w+)%1(\\w+)%2(\\w+)").arg(formatDelimiters[0],
                                                                  formatDelimiters[1]));
    } else {
        inputrex.setPattern("(\\w+)\\W+(\\w+)\\W+(\\w+)");
    }

    if (inputrex.indexIn(_in) == -1) {
        throw ALKEXCEPTION("Invalid input string");
    }

    QStringList scannedParts;
    scannedParts += inputrex.cap(1).toLower();
    scannedParts += inputrex.cap(2).toLower();
    scannedParts += inputrex.cap(3).toLower();

    //
    // Convert the scanned parts into actual date components
    //
    unsigned day = 0, month = 0, year = 0;
    bool ok;
    QRegExp digitrex("(\\d+)");
    QStringList::const_iterator it_scanned = scannedParts.constBegin();
    QStringList::const_iterator it_format = formatParts.constBegin();
    while (it_scanned != scannedParts.constEnd()) {
        // decide upon the first character of the part
        switch ((*it_format).at(0).cell()) {
        case 'd':
            // remove any extraneous non-digits (e.g. read "3rd" as 3)
            ok = false;
            if (digitrex.indexIn(*it_scanned) != -1) {
                day = digitrex.cap(1).toUInt(&ok);
            }
            if (!ok || day > 31) {
                throw ALKEXCEPTION(QString("Invalid day entry: %1").arg(*it_scanned));
            }
            break;
        case 'm':
            month = (*it_scanned).toUInt(&ok);
            if (!ok) {
                // maybe it's a textual date
                unsigned i = 1;
                while (i <= 12) {
                    if (KGlobal::locale()->calendar()->monthName(i, 2000).toLower() == *it_scanned
                        || KGlobal::locale()->calendar()->monthName(i, 2000,
                                                                    KCalendarSystem::ShortName).
                        toLower() == *it_scanned) {
                        month = i;
                    }
                    ++i;
                }
            }

            if (month < 1 || month > 12) {
                throw ALKEXCEPTION(QString("Invalid month entry: %1").arg(*it_scanned));
            }

            break;
        case 'y':
            if (_strict && (*it_scanned).length() != (*it_format).length()) {
                throw ALKEXCEPTION(QString(
                                       "Length of year (%1) does not match expected length (%2).")
                                   .arg(*it_scanned, *it_format));
            }

            year = (*it_scanned).toUInt(&ok);

            if (!ok) {
                throw ALKEXCEPTION(QString("Invalid year entry: %1").arg(*it_scanned));
            }

            //
            // 2-digit year case
            //
            // this algorithm will pick a year within +/- 50 years of the
            // centurymidpoint parameter.  i.e. if the midpoint is 2000,
            // then 0-49 will become 2000-2049, and 50-99 will become 1950-1999
            if (year < 100) {
                unsigned centuryend = _centurymidpoint + 50;
                unsigned centurybegin = _centurymidpoint - 50;

                if (year < centuryend % 100) {
                    year += 100;
                }
                year += centurybegin - centurybegin % 100;
            }

            if (year < 1900) {
                throw ALKEXCEPTION(QString("Invalid year (%1)").arg(year));
            }

            break;
        default:
            throw ALKEXCEPTION("Invalid format character");
        }

        ++it_scanned;
        ++it_format;
    }
    QDate result(year, month, day);
    if (!result.isValid()) {
        throw ALKEXCEPTION(QString("Invalid date (yr%1 mo%2 dy%3)").arg(year).arg(month).arg(day));
    }

    return result;
}

#else
#include <QLocale>
#include <QRegularExpression>

const QDate AlkDateFormat::convertStringKMyMoney(const QString& _in, bool _strict, unsigned _centurymidpoint) const
{
  //
  // Break date format string into component parts
  //

  QRegularExpression formatrex("%([mdy]+)(\\W+)%([mdy]+)(\\W+)%([mdy]+)", QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatch match;
  if (m_format.indexOf(formatrex, 0, &match) == -1) {
    throw ALKEXCEPTION("Invalid format string");
  }

  QStringList formatParts;
  formatParts += match.captured(1);
  formatParts += match.captured(3);
  formatParts += match.captured(5);

  QStringList formatDelimiters;
  formatDelimiters += match.captured(2);
  formatDelimiters += match.captured(4);
  match = QRegularExpressionMatch();

  //
  // Break input string up into component parts,
  // using the delimiters found in the format string
  //

  QRegularExpression inputrex;
  inputrex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

  // strict mode means we must enforce the delimiters as specified in the
  // format.  non-strict allows any delimiters
  if (_strict)
    inputrex.setPattern(QString("(\\w+)%1(\\w+)%2(\\w+)").arg(formatDelimiters[0], formatDelimiters[1]));
  else
    inputrex.setPattern("(\\w+)\\W+(\\w+)\\W+(\\w+)");

  if (_in.indexOf(inputrex, 0, &match) == -1) {
    throw ALKEXCEPTION("Invalid input string");
  }

  QStringList scannedParts;
  scannedParts += match.captured(1).toLower();
  scannedParts += match.captured(2).toLower();
  scannedParts += match.captured(3).toLower();
  match = QRegularExpressionMatch();

  //
  // Convert the scanned parts into actual date components
  //
  unsigned day = 0, month = 0, year = 0;
  bool ok;
  QRegularExpression digitrex("(\\d+)");
  QStringList::const_iterator it_scanned = scannedParts.constBegin();
  QStringList::const_iterator it_format = formatParts.constBegin();
  while (it_scanned != scannedParts.constEnd()) {
    // decide upon the first character of the part
    switch ((*it_format).at(0).cell()) {
      case 'd':
        // remove any extraneous non-digits (e.g. read "3rd" as 3)
        ok = false;
        if ((*it_scanned).indexOf(digitrex, 0, &match) != -1)
          day = match.captured(1).toUInt(&ok);
        if (!ok || day > 31)
          throw ALKEXCEPTION(QString("Invalid day entry: %1").arg(*it_scanned));
        break;
      case 'm':
        month = (*it_scanned).toUInt(&ok);
        if (!ok) {
          month = 0;
          // maybe it's a textual date
          unsigned i = 1;
          // search the name in the current selected locale
          QLocale locale;
          while (i <= 12) {
            if (locale.standaloneMonthName(i).toLower() == *it_scanned
                || locale.standaloneMonthName(i, QLocale::ShortFormat).toLower() == *it_scanned) {
              month = i;
              break;
            }
            ++i;
          }
          // in case we did not find the month in the current locale,
          // we look for it in the C locale
          if(month == 0) {
            QLocale localeC(QLocale::C);
            if( !(locale == localeC)) {
              i = 1;
              while (i <= 12) {
                if (localeC.standaloneMonthName(i).toLower() == *it_scanned
                    || localeC.standaloneMonthName(i, QLocale::ShortFormat).toLower() == *it_scanned) {
                  month = i;
                  break;
                }
                ++i;
              }
            }
          }
        }

        if (month < 1 || month > 12)
          throw ALKEXCEPTION(QString("Invalid month entry: %1").arg(*it_scanned));

        break;
      case 'y':
        if (_strict && (*it_scanned).length() != (*it_format).length())
          throw ALKEXCEPTION(QString("Length of year (%1) does not match expected length (%2).")
                                 .arg(*it_scanned, *it_format));

        year = (*it_scanned).toUInt(&ok);

        if (!ok)
          throw ALKEXCEPTION(QString("Invalid year entry: %1").arg(*it_scanned));

        //
        // 2-digit year case
        //
        // this algorithm will pick a year within +/- 50 years of the
        // centurymidpoint parameter.  i.e. if the midpoint is 2000,
        // then 0-49 will become 2000-2049, and 50-99 will become 1950-1999
        if (year < 100) {
          unsigned centuryend = _centurymidpoint + 50;
          unsigned centurybegin = _centurymidpoint - 50;

          if (year < centuryend % 100)
            year += 100;
          year += centurybegin - centurybegin % 100;
        }

        if (year < 1900)
          throw ALKEXCEPTION(QString("Invalid year (%1)").arg(year));

        break;
      default:
        throw ALKEXCEPTION("Invalid format character");
    }

    ++it_scanned;
    ++it_format;
  }
  QDate result(year, month, day);
  if (! result.isValid())
    throw ALKEXCEPTION(QString("Invalid date (yr%1 mo%2 dy%3)").arg(year).arg(month).arg(day));

  return result;
}
#endif
