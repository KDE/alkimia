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

#ifndef ALKQUOTERECEIVER_H
#define ALKQUOTERECEIVER_H

#include "alkmoney.h"

#include <QDate>
#include <QObject>
#include <QStringList>

class AlkOnlineQuote;

namespace convertertest {
/**
Simple class to handle signals/slots for unit tests

@author Ace Jones <acejones@users.sourceforge.net>
*/
class ALK_NO_EXPORT AlkQuoteReceiver : public QObject
{
    Q_OBJECT
public:
    explicit AlkQuoteReceiver(AlkOnlineQuote *q, QObject *parent = 0);
    ~AlkQuoteReceiver();
    void setVerbose(bool verbose)
    {
        m_verbose = verbose;
    }

public Q_SLOTS:
    void slotGetQuote(const QString &, const QString &, const QDate &, const double &);
    void slotStatus(const QString &);
    void slotError(const QString &);
public:
    QStringList m_statuses;
    QStringList m_errors;
    AlkMoney m_price;
    QDate m_date;
protected:
    bool m_verbose;
};
} // end namespace convertertest

#endif // ALKQUOTERECEIVER_H
