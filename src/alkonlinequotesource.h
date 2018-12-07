/***************************************************************************
 *   Copyright 2018  Ralf Habacker <ralf.habacker@freenet.de>              *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
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

#ifndef ALKONLINEQUOTESOURCE_H
#define ALKONLINEQUOTESOURCE_H

#include <alkimia/alkonlinequotesprofile.h>

#include <QString>

/**
  * @author Thomas Baumgart & Ace Jones
  *
  * This is a helper class to store information about an online source
  * for stock prices or currency exchange rates.
  */
class ALK_EXPORT AlkOnlineQuoteSource
{
public:
    AlkOnlineQuoteSource();
    AlkOnlineQuoteSource(const AlkOnlineQuoteSource &other);
    AlkOnlineQuoteSource &operator=(const AlkOnlineQuoteSource &other);
    AlkOnlineQuoteSource(const QString &name, AlkOnlineQuotesProfile *profile);
    AlkOnlineQuoteSource(const QString &name, const QString &url, const QString &sym,
                         const QString &price, const QString &date, const QString &dateformat,
                         bool skipStripping = false);
    ~AlkOnlineQuoteSource();
    bool isValid();

    bool read();
    bool write();
    void rename(const QString &name);
    void remove();

    QString name() const;
    QString url() const;
    QString sym() const;
    QString price() const;
    QString date() const;
    QString dateformat() const;
    bool skipStripping() const;
    bool isGHNS();

    void setName(const QString &name);
    void setUrl(const QString &url);
    void setSym(const QString &symbol);
    void setPrice(const QString &price);
    void setDate(const QString &date);
    void setDateformat(const QString &dateformat);
    void setSkipStripping(bool state);
    void setGHNS(bool state);

    QString ghnsWriteFileName();
    void setProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfile *profile();

protected:
    class Private;
    Private *const d;
};

#endif // ALKONLINEQUOTESOURCE_H
