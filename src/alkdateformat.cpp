/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2018-2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkdateformat.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <KGlobal>
#include <KCalendarSystem>
#include <QRegExp>
#else
#include <QLocale>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#endif


class AlkDateFormat::Private
{
public:
    QString                     m_format;
    AlkDateFormat::ErrorCode    m_errorCode;
    QString                     m_errorMessage;

    QDate setError(AlkDateFormat::ErrorCode errorCode, const QString& arg1 = QString(), const QString& arg2 = QString())
    {
        m_errorCode = errorCode;
        switch(errorCode) {
            case AlkDateFormat::NoError:
                m_errorMessage.clear();
                break;
            case AlkDateFormat::InvalidFormatString:
                m_errorMessage = QString("Invalid format string '%1'").arg(arg1);
                break;
            case AlkDateFormat::InvalidFormatCharacter:
                m_errorMessage = QString("Invalid format character '%1'").arg(arg1);
                break;
            case AlkDateFormat::InvalidDate:
                m_errorMessage = QString("Invalid date '%1'").arg(arg1);
                break;
            case AlkDateFormat::InvalidDay:
                m_errorMessage = QString("Invalid day entry: %1").arg(arg1);
                break;
            case AlkDateFormat::InvalidMonth:
                m_errorMessage = QString("Invalid month entry: %1").arg(arg1);
                break;
            case AlkDateFormat::InvalidYear:
                m_errorMessage = QString("Invalid year entry: %1").arg(arg1);
                break;
            case AlkDateFormat::InvalidYearLength:
                m_errorMessage = QString("Length of year (%1) does not match expected length (%2).").arg(arg1, arg2);
                break;
        }
        return QDate();
    }

    QDate convertStringSkrooge(const QString &_in)
    {
        QDate date;
        if (m_format == "UNIX") {
            bool ok;
            const quint64 unixTime = _in.toUInt(&ok);
            if (ok) {
                date = QDateTime::fromMSecsSinceEpoch(unixTime * 1000, Qt::UTC).date();
            }

        } else {
            const QString skroogeFormat = m_format;

            m_format = m_format.toLower();

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
            QRegExp formatrex("([mdy]+)(\\W+)([mdy]+)(\\W+)([mdy]+)", Qt::CaseInsensitive);
            if (formatrex.indexIn(m_format) == -1) {
                return setError(AlkDateFormat::InvalidFormatString, m_format);
            }
            m_format = QLatin1String("%");
            m_format.append(formatrex.cap(1));
            m_format.append(formatrex.cap(2));
            m_format.append(QLatin1String("%"));
            m_format.append(formatrex.cap(3));
            m_format.append(formatrex.cap(4));
            m_format.append(QLatin1String("%"));
            m_format.append(formatrex.cap(5));
#else
            static QRegularExpression formatrex("([mdy]+)(\\W+)([mdy]+)(\\W+)([mdy]+)", QRegularExpression::CaseInsensitiveOption);
            auto match = formatrex.match(m_format);
            if (!match.hasMatch()) {
                return setError(AlkDateFormat::InvalidFormatString, m_format);
            }

            m_format = QLatin1String("%");
            m_format += match.captured(1);
            m_format += match.captured(2);
            m_format.append(QLatin1String("%"));
            m_format += match.captured(3);
            m_format += match.captured(4);
            m_format.append(QLatin1String("%"));
            m_format += match.captured(5);
#endif
            date = convertStringKMyMoney(_in, true, 2000);
            m_format = skroogeFormat;
        }
        if (!date.isValid()) {
            return setError(AlkDateFormat::InvalidDate, _in);
        }
        if (!m_format.contains(QStringLiteral("yyyy")) && date.year() < 2000)
            date = date.addYears(100);
        return date;
    }

    QDate convertStringUnix(const QString& _in)
    {
        bool ok;
        quint64 unixTime = _in.toUInt(&ok);
        if (!ok) {
            return setError(AlkDateFormat::InvalidDate, _in);
        }
        if (m_format.startsWith(QLatin1String("%ud"))) {
            unixTime *= 86400; // times seconds per day
        }
        return QDateTime::fromMSecsSinceEpoch(unixTime * 1000, Qt::UTC).date();
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

    QDate convertStringKMyMoney(const QString &_in, bool _strict, unsigned _centurymidpoint)
    {
        if (m_format.startsWith(QLatin1String("%u"))) {
            return convertStringUnix(_in);
        }

        //
        // Break date format string into component parts
        //

        QRegExp formatrex("%([mdy]+)(\\W+)%([mdy]+)(\\W+)%([mdy]+)", Qt::CaseInsensitive);
        if (formatrex.indexIn(m_format) == -1) {
            return setError(AlkDateFormat::InvalidFormatString, m_format);
        }

        QStringList formatParts;
        formatParts += formatrex.cap(1);
        formatParts += formatrex.cap(3);
        formatParts += formatrex.cap(5);

        QStringList formatDelimiters;
        formatDelimiters += formatrex.cap(2);
        formatDelimiters += formatrex.cap(4);

        // make sure to escape delimiters that are special chars in regex
        QStringList::iterator it;
        QRegExp specialChars("^[\\.\\\\\\?]$");
        for(it = formatDelimiters.begin(); it != formatDelimiters.end(); ++it) {
            if (specialChars.indexIn(*it) != -1)
                (*it).prepend("\\");
        }

        //
        // Break input string up into component parts,
        // using the delimiters found in the format string
        //

        QRegExp inputrex;
        inputrex.setCaseSensitivity(Qt::CaseInsensitive);

        // strict mode means we must enforce the delimiters as specified in the
        // format.  non-strict allows any delimiters
        if (_strict) {
            inputrex.setPattern(QString("(\\w+)\\.?%1(\\w+)\\.?%2(\\w+)\\.?").arg(formatDelimiters[0],
                                                                                  formatDelimiters[1]));
        } else {
            inputrex.setPattern("(\\w+)\\W+(\\w+)\\W+(\\w+)");
        }

        if (inputrex.indexIn(_in) == -1) {
            return setError(AlkDateFormat::InvalidDate, _in);
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
                    return setError(AlkDateFormat::InvalidDay, *it_scanned);
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
                    return setError(AlkDateFormat::InvalidMonth, *it_scanned);
                }

                break;
            case 'y':
                if (_strict && (*it_scanned).length() != (*it_format).length()) {
                    return setError(AlkDateFormat::InvalidYearLength, *it_scanned, *it_format);
                }

                year = (*it_scanned).toUInt(&ok);

                if (!ok) {
                    return setError(AlkDateFormat::InvalidYear, *it_scanned);
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
                    return setError(AlkDateFormat::InvalidYear, QString::number(year));
                }

                break;
            default:
                return setError(AlkDateFormat::InvalidFormatCharacter, QString((*it_format).at(0).cell()));
            }

            ++it_scanned;
            ++it_format;
        }
        QDate result(year, month, day);
        if (!result.isValid()) {
            return setError(AlkDateFormat::InvalidDate, QString("yr:%1 mo:%2 dy:%3)").arg(year).arg(month).arg(day));
        }

        return result;
    }

#else // Qt5

    QDate convertStringKMyMoney(const QString& _in, bool _strict, unsigned _centurymidpoint)
    {
      if (m_format.startsWith(QLatin1String("%u"))) {
        return convertStringUnix(_in);
      }

      //
      // Break date format string into component parts
      //

      QRegularExpression formatrex("%([mdy]+)(\\W+)%([mdy]+)(\\W+)%([mdy]+)", QRegularExpression::CaseInsensitiveOption);
      QRegularExpressionMatch match = formatrex.match(m_format);
      if (!match.hasMatch()) {
        return setError(AlkDateFormat::InvalidFormatString, m_format);
      }

      QStringList formatParts;
      formatParts += match.captured(1);
      formatParts += match.captured(3);
      formatParts += match.captured(5);

      QStringList formatDelimiters;
      formatDelimiters += match.captured(2);
      formatDelimiters += match.captured(4);

      // make sure to escape delimiters that are special chars in regex
      QStringList::iterator it;
      QRegularExpression specialChars("^[\\.\\\\\\?]$");
      for(it = formatDelimiters.begin(); it != formatDelimiters.end(); ++it) {
        QRegularExpressionMatch special = specialChars.match(*it);
        if (special.hasMatch()) {
          (*it).prepend("\\");
        }
      }

      //
      // Break input string up into component parts,
      // using the delimiters found in the format string
      //
      QRegularExpression inputrex;
      inputrex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

      // strict mode means we must enforce the delimiters as specified in the
      // format.  non-strict allows any delimiters
      if (_strict)
        inputrex.setPattern(QString("(\\w+)\\.?%1(\\w+)\\.?%2(\\w+)\\.?").arg(formatDelimiters[0], formatDelimiters[1]));
      else
        inputrex.setPattern("(\\w+)\\W+(\\w+)\\W+(\\w+)");

      match = inputrex.match(_in);
      if (!match.hasMatch()) {
        return setError(AlkDateFormat::InvalidDate, _in);
      }

      QStringList scannedParts;
      scannedParts += match.captured(1).toLower();
      scannedParts += match.captured(2).toLower();
      scannedParts += match.captured(3).toLower();

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
            match = digitrex.match(*it_scanned);
            if (match.hasMatch())
              day = match.captured(1).toUInt(&ok);
            if (!ok || day > 31)
              return setError(AlkDateFormat::InvalidDay, *it_scanned);
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
              return setError(AlkDateFormat::InvalidMonth, *it_scanned);

            break;
          case 'y':
            if (_strict && (*it_scanned).length() != (*it_format).length())
              return setError(AlkDateFormat::InvalidYearLength, *it_scanned, *it_format);

            year = (*it_scanned).toUInt(&ok);

            if (!ok)
              return setError(AlkDateFormat::InvalidYear, *it_scanned);

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
              return setError(AlkDateFormat::InvalidYear, QString::number(year));

            break;
          default:
            return setError(AlkDateFormat::InvalidFormatCharacter, QString(QChar((*it_format).at(0).cell())));
        }

        ++it_scanned;
        ++it_format;
      }
      QDate result(year, month, day);
      if (! result.isValid())
        return setError(AlkDateFormat::InvalidDate, QString("yr:%1 mo:%2 dy:%3)").arg(year).arg(month).arg(day));

      return result;
    }
#endif

};

AlkDateFormat::AlkDateFormat(const QString &format)
    : d(new Private)
{
    d->m_format = format;
    d->m_errorCode = NoError;
}

AlkDateFormat::AlkDateFormat(const AlkDateFormat& right)
    : d(new Private)
{
    d->m_format = right.d->m_format;
    d->m_errorCode = NoError;
}

AlkDateFormat::~AlkDateFormat()
{
    delete d;
}

AlkDateFormat& AlkDateFormat::operator=(const AlkDateFormat& right)
{
    d->m_format = right.d->m_format;

    return *this;
}

const QString & AlkDateFormat::format() const
{
    return d->m_format;
}

AlkDateFormat::ErrorCode AlkDateFormat::lastError() const
{
    return d->m_errorCode;
}

QString AlkDateFormat::lastErrorMessage() const
{
    return d->m_errorMessage;
}


QDate AlkDateFormat::convertString(const QString& date, bool strict, unsigned int centuryMidPoint)
{
    // reset any pending errors from previous runs
    d->m_errorCode = NoError;
    d->m_errorMessage.clear();

    if (d->m_format.contains("%"))
        return d->convertStringKMyMoney(date, strict, centuryMidPoint);
    else
        return d->convertStringSkrooge(date);
}

QString AlkDateFormat::convertDate(const QDate& date)
{
    Q_UNUSED(date);

    // reset any pending errors from previous runs
    d->m_errorCode = NoError;
    d->m_errorMessage.clear();

    return QString();
}
