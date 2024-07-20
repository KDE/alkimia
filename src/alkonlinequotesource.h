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
        CSV,
        CSS,
    };

    /**
     * Type of decimal separator
     */
    enum DecimalSeparator {
        Legacy,
        Period,
        Comma
    };

    enum DownloadType {
        Default,
        Javascript,
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
                                  DataFormat dataFormat = HTML,
                                  DecimalSeparator priceDecimalSeparator = Legacy,
                                  DownloadType downloadType = Default);
    ~AlkOnlineQuoteSource();

    AlkOnlineQuoteSource(const AlkOnlineQuoteSource &other);
    AlkOnlineQuoteSource &operator=(AlkOnlineQuoteSource other);

    static AlkOnlineQuoteSource defaultCurrencyQuoteSource(const QString& name);
    static AlkOnlineQuoteSource testQuoteSource(const QString &name, bool twoSymbols = false, DownloadType downloadType = Default, DataFormat format = HTML);

    friend void swap(AlkOnlineQuoteSource& first, AlkOnlineQuoteSource& second);

    /**
     * Return referenced quote source
     *
     * If this source is not a reference, an empty source is returned.
     *
     * @return referenced quote source or empty source otherwise
     */
    AlkOnlineQuoteSource asReference() const;

    /**
     * Return state if this source is a reference
     *
     * @return true the current source is a reference
     * @return false the current source is not a reference
     */
    bool isReference() const;
    bool isEmpty();
    bool isValid();

    bool read();
    bool write();
    void rename(const QString &name);
    void remove();

    /**
     * Return name of the referenced quote source
     *
     * @return referenced source name
     */
    QString referenceName() const;
    QString name() const;
    QString url() const;
    DecimalSeparator priceDecimalSeparator() const;
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
    DownloadType downloadType() const;
    QString financeQuoteName() const;
    IdSelector idSelector() const;

    bool isGHNS() const;
    bool isReadOnly() const;
    bool isFinanceQuote() const;
    static bool isFinanceQuote(const QString &name);

    /**
     * Make this source a reference
     *
     * Calling this method makes this source a reference to a remote online quote source.
     *
     *@param name name of the referenced source
     */
    void setReferenceName(const QString &name);
    void setName(const QString &name);
    void setUrl(const QString &url);
    void setPriceDecimalSeparator(DecimalSeparator separator);
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
    void setDownloadType(DownloadType downloadType);
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

/**
 * Returns the specified data format as text
 * @param format data format for which the text is to be returned
 * @return data format as text
 */
ALK_EXPORT QString toString(AlkOnlineQuoteSource::DataFormat format);

#endif // ALKONLINEQUOTESOURCE_H
