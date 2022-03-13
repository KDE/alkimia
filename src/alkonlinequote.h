/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2020 Thomas Baumgart <tbaumgart@kde.org>

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKONLINEQUOTE_H
#define ALKONLINEQUOTE_H

#include <alkimia/alk_export.h>

#include <QObject>
#include <QDateTime>
#include <QString>

class AlkOnlineQuotesProfile;

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
    class Errors
    {
    public:
        enum Type {
            None,
            Data,
            Date,
            DateFormat,
            Price,
            Script,
            Source,
            Symbol,
            Success,
            URL,
        };

        Errors();
        Errors(Type type);
        Errors(const Errors& e) = default;
        Errors& operator|=(Type t);
        bool operator &(Type t) const;

    protected:
        QList<Type> m_type;
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
      * This launches a web-based quote update for the given @p _symbol.
      * When the quote is received back from the web source, it will be
      * emitted on the 'quote' signal.
      *
      * If services do not provide a date, parsing of the date can be disabled
      * by specifying an empty date attribute of the given online source.
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
public slots:
    bool launch(const QString &_symbol, const QString &_id, const QString &_source = QString());

    /**
      * If @ref launch() returns false, this method can be used to get details
      * about the errors that occurred.
      *
      * @return bit map of errors, see class @ref Errors for details
     */
    const Errors &errors();

signals:
    void quote(QString id, QString symbol, QDate date, double price);
    void failed(QString id, QString symbol);
    void status(QString s);
    void error(QString s);

private:
    class Private;
    Private *const d;
};

#endif // ALKONLINEQUOTE_H
