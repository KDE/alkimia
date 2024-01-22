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

    /**
     * Supported formats of downloaded data
     */
    enum DataFormat {
        StrippedHTML,
        HTML,
        CSV
    };

    AlkOnlineQuoteSource();
    explicit AlkOnlineQuoteSource(const QString &name, AlkOnlineQuotesProfile *profile);
    explicit AlkOnlineQuoteSource(const QString& name,
                                  const QString& url,
                                  const QString& idRegex,
                                  const IdSelector idBy,
                                  const QString& priceRegex,
                                  const QString& dateRegex,
                                  const QString& dateFormat,
                                  DataFormat dataFormat = HTML);
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
    /**
     * Return the format of the downloaded data
     *
     * @return Format identifier
     */
    DataFormat dataFormat() const;
    QString dateRegex() const;
    QString dateFormat() const;
    QString financeQuoteName() const;
    IdSelector idSelector() const;

    bool isGHNS() const;
    bool isReadOnly() const;
    bool isFinanceQuote() const;
    static bool isFinanceQuote(const QString &name);

    void setName(const QString &name);
    void setUrl(const QString &url);
    void setPriceRegex(const QString &priceRegex);
    void setIdRegex(const QString &idRegex);

    /**
     * Set the format of the downloaded data
     *
     * @param dataFormat Format identifier
     */
    void setDataFormat(DataFormat dataFormat);
    void setDateRegex(const QString &dateRegex);
    void setDateFormat(const QString &dateFormat);
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
    QString ghnsWriteFileName() const;
    void setProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfile *profile();
    AlkOnlineQuotesProfile *profile() const;

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
