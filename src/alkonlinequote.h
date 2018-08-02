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

#ifndef ALKONLINEQUOTE_H
#define ALKONLINEQUOTE_H

#include "alkmoney.h"
#include "alkonlinequotesource.h"

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QMap>

#include <KProcess>
#include <KUrl>

class QWebView;

/**
Retrieves a price quote from a web-based quote source

@author Ace Jones <acejones@users.sourceforge.net>
*/
class ALK_EXPORT AlkOnlineQuote : public QObject
{
    Q_OBJECT
public:
    explicit AlkOnlineQuote(QObject * = 0);
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
        Errors(const Errors &e);
        Errors &operator |=(Type t);
        bool operator &(Type t) const;

    protected:
        QList<Type> m_type;
    };

    typedef enum _quoteSystemE {
        Native = 0,
        FinanceQuote
    } quoteSystemE;

    /**
     * Set webkit view for fetching web urls using javascript
     *
     * @param view webkit view to set
     */
    void setWebView(QWebView *view);

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
      * If @ref launch(), @ref launchNative() or @ref launchFinanceQuote() returns false,
      * this method can be used to get details about the errors that occurred.
      *
      * @return bit map of errors, see class @ref Errors for details
     */
    const Errors &errors();

    /**
      * This returns a list of the names of the quote sources
      * currently defined.
      *
     * @param _system whether to return Native or Finance::Quote source list
     * @return QStringList of quote source names
      */
    static const QStringList quoteSources(const _quoteSystemE _system = Native);

signals:
    void quote(const QString &, const QString &, const QDate &, const double &);
    void failed(const QString &, const QString &);
    void status(const QString &);
    void error(const QString &);

private:
    class Private;
    Private *d;
};

#endif // WEBPRICEQUOTE_H
