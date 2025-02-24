/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2020 Thomas Baumgart <tbaumgart@kde.org>

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKONLINEQUOTE_H
#define ALKONLINEQUOTE_H

#include <alkimia/alkvalue.h>

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QString>

class AlkOnlineQuoteSource;
class AlkOnlineQuotesProfile;

typedef QMap<QDate, AlkValue> AlkDatePriceMap;

/**
Retrieves a price quote from a web-based quote source

@author Ace Jones acejones @users.sourceforge.net
*/
class ALK_EXPORT AlkOnlineQuote : public QObject
{
    Q_OBJECT
public:
    explicit AlkOnlineQuote(AlkOnlineQuotesProfile *profile = 0, QObject * = 0);
    ~AlkOnlineQuote();

    /**
     * Hold errors reported from price quote fetching and parsing
     *
     * The implementation provides a type safe way to use
     * bit operations like '|=' for combining values and '&'
     * for checking value presence.
     */
    class ALK_EXPORT Errors
    {
    public:
        enum Type {
            None,
            Data,
            Date,
            DatePattern,
            DateFormat,
            Price,
            PricePattern,
            Script,
            Source,
            Symbol,
            Success,
            Timeout,
            URL,
        };

        Errors();
        Errors(Type type);
        Errors& operator|=(Type t);
        bool operator &(Type t) const;
        bool operator ==(Type t) const;
        bool operator !=(Type t) const;
        bool isEmpty() const;

    protected:
        QList<Type> m_type;
    };

    /**
     * Supported values for returning prices in special cases
     */
    enum LastPriceState {
        /**
         * No handling of special cases
         */
        Off,
        /**
         *  If no price is available in the specified period,
         *  but older ones are available, the most current price
         *  is returned.
         */
        Always,
        /**
         * If the date range has the same start and end date,
         * is identical to the current date, no price was
         * found but older ones are available, the most current
         * price is returned. (Default)
         */
        AlwaysWhenToday
    };

    AlkOnlineQuotesProfile *profile();
    void setProfile(AlkOnlineQuotesProfile *profile);

    /**
     * Set accepted language the online quote should be returned for
     *
     * @param language accepted language to set
     */
    void setAcceptLanguage(const QString &language);

    /**
     * Return actual used timeout for fetching online quotes
     * If the returned value is -1, no timeout has been set.
     * @return timeout
     */
    int timeout() const;

    /**
     * Set timeout for fetching online quotes
     * If the timeout is != -1, a request to retrieve online quotes will be aborted
     * if the time set with this function has been exceeded.
     * @param newTimeout timeout in millseconds
     */
    void setTimeout(int newTimeout);

    /**
     * Defines a date range within which the data is to be retrieved.
     * This range is only taken into account for data in CSV format and
     * provides online quotes via the `quotes` signal.
     * @param from first date to include the online quote
     * @param to last date to include the online quote
     */
    void setDateRange(const QDate &from, const QDate &to);

    /**
     * Returns the status of the price to be returned for special date ranges.
     *
     * See setReturnLastPriceState() for details.
     *
     * @return current state used, see AlkOnlineQuote::LastPriceState for the supported values.
     */
    LastPriceState returnLastPriceState();

    /**
     * Sets the status of the price to be returned for special date range.
     *
     * This setting is intended for handling special cases when returning
     * prices and rates, for example in cases where the time range for a
     * price query does not match the delivered prices, e.g. because only
     * today was specified and the last available price is from the day
     * before yesterday.
     *
     * @param state the state to use, see AlkOnlineQuote::LastPriceState for the supported values.
     */
    void setReturnLastPriceState(LastPriceState state);

    /**
     * Always use signal `quote`.
     * For online sources in CSV data format (see AlkOnlineQuoteSource::DataFormat),
     * the quotes found are normally returned via the `quotes` signal,
     * which can be changed to the `quote` signal using this method.
     *
     * This means that sources in CSV format can be processed via the
     * same interface as other formats.
     * @param state Enable or disable use of the `quote` signal
     */
    void setUseSingleQuoteSignal(bool state);

    /**
     * Returns the status of whether the `quote` signal is always used
     * @return current state
     */
    bool useSingleQuoteSignal();

    /**
     * Returns the status whether a search with swapped symbols should
     * be performed after a query for a symbol returned nothing.
     * @return current state
     */
    bool enableReverseLaunch();

    /**
     * Set the status whether a search with swapped symbole should be
     * performed after a query for a symbol returned nothing.
     * @param state Enable or disable search with swapped symbols
     * @note The reverse search is enabled by default
     */
    void setEnableReverseLaunch(bool state);

    /**
     * Provides the currently used online source.
     * @return online quote source
     */
    const AlkOnlineQuoteSource &source() const;

public Q_SLOTS:
    /**
     * This launches a web-based quote update for the given @p _symbol.
     * When the quote is received back from the web source, it will be
     * emitted on the 'quote' signal.
     *
     * If services do not provide a date, parsing of the date can be disabled
     * by specifying an empty date attribute of the given online source.
     *
     * If a timeout is set with @ref setTimeout() and the web source does not
     * return the requested data, the update will be aborted after this time.
     *
     * @param _symbol the trading symbol of the stock to fetch a price for
     * @param _id an arbitrary identifier, which will be emitted in the quote
     *                signal when a price is sent back.
     * @param _source the source of the quote (must be a valid value returned
     *                by quoteSources().  Send QString() to use the default
     *                source.
     * @return bool Whether the quote fetch process was launched successfully
     *              In case of failures it returns false and @ref errors()
     *              could be used to get error details.
     */
    bool launch(const QString &_symbol, const QString &_id, const QString &_source = QString());

    /**
     * If @ref launch() returns false, this method can be used to get details
     * about the errors that occurred.
     *
     * @return bit map of errors, see class @ref Errors for details
     */
    const AlkOnlineQuote::Errors &errors();

Q_SIGNALS:
    void quote(QString id, QString symbol, QDate date, double price);
    void quotes(const QString &id, const QString &symbol, const AlkDatePriceMap &prices);
    void failed(QString id, QString symbol);
    void status(QString s);
    void error(QString s);

private:
    class Private;
    Private *const d;

protected:
    Private &d_ptr();

    friend class AlkOnlineQuotePrivateTest;
};

Q_DECLARE_METATYPE(AlkOnlineQuote::LastPriceState)

#endif // ALKONLINEQUOTE_H
