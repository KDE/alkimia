/*
 * SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker@freenet.de
 * SPDX-FileCopyrightText: 2023 Thomas Baumgart <tbaumgart@kde.org>
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of libalkimia.
 */

#ifndef ALKONLINEQUOTESOURCE_H
#define ALKONLINEQUOTESOURCE_H

#include <alkimia/alkonlinequotesprofile.h>

#include <QString>

class AlkOnlineQuotesProfile;

/**
  * @author Thomas Baumgart & Ace Jones
  *
  * This is a helper class to store information about an online source
  * for stock prices or currency exchange rates.
  */
class ALK_EXPORT AlkOnlineQuoteSource
{
public:
    enum IdSelector {
        Symbol,
        IdentificationNumber,
        Name,
    };

    AlkOnlineQuoteSource();
    explicit AlkOnlineQuoteSource(const QString &name, AlkOnlineQuotesProfile *profile);
    explicit AlkOnlineQuoteSource(const QString& name,
                                  const QString& url,
                                  const QString& idNumber,
                                  const IdSelector idBy,
                                  const QString& price,
                                  const QString& date,
                                  const QString& dateFormat,
                                  bool skipStripping = false);
    ~AlkOnlineQuoteSource();

    AlkOnlineQuoteSource(const AlkOnlineQuoteSource &other);
    AlkOnlineQuoteSource &operator=(AlkOnlineQuoteSource other);

    static AlkOnlineQuoteSource defaultCurrencyQuoteSource(const QString& name);

    friend void swap(AlkOnlineQuoteSource& first, AlkOnlineQuoteSource& second);

    bool isEmpty();
    bool isValid();

    bool read();
    bool write();
    void rename(const QString &name);
    void remove();

    QString name() const;
    QString url() const;
    QString priceRegex() const;
    QString idRegex() const;
    QString dateRegex() const;
    QString dateFormat() const;
    QString financeQuoteName() const;
    IdSelector idSelector() const;

    bool skipStripping() const;
    bool isGHNS();
    bool isReadOnly();
    bool isFinanceQuote() const;
    static bool isFinanceQuote(const QString &name);

    void setName(const QString &name);
    void setUrl(const QString &url);
    void setPriceRegex(const QString &priceRegex);
    void setIdRegex(const QString &idRegex);
    void setDateRegex(const QString &dateRegex);
    void setDateFormat(const QString &dateFormat);
    void setSkipStripping(bool state);
    void setGHNS(bool state);
    void setIdSelector(IdSelector idSelector);

    /**
     * Return the default identifier known to work
     * @return default identifier
     */
    const QString &defaultId() const;

    /**
     * Set the default identifier, which is known to work
     * @param defaultID default identifier
     */
    void setDefaultId(const QString &defaultId);

    /**
     * Return state if this source requires two identifier
     * @return false required one identifier
     * @return true required two identifier
     */
    bool requiresTwoIdentifier() const;
    QString ghnsWriteFileName();
    void setProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfile *profile();

protected:
    class Private;
    Private *d;
};

inline void swap(AlkOnlineQuoteSource& first, AlkOnlineQuoteSource& second) // krazy:exclude=inline
{
  using std::swap;
  swap(first.d, second.d);
}

#endif // ALKONLINEQUOTESOURCE_H
