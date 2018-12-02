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

#include "alkquotereceiver.h"
#include "alkonlinequote.h"

#include <QtDebug>

convertertest::AlkQuoteReceiver::AlkQuoteReceiver(AlkOnlineQuote *q, QObject *parent)
    : QObject(parent)
{
    connect(q, SIGNAL(quote(QString,QString,QDate,double)),
            this, SLOT(slotGetQuote(QString,QString,QDate,double)));
    connect(q, SIGNAL(status(QString)),
            this, SLOT(slotStatus(QString)));
    connect(q, SIGNAL(error(QString)),
            this, SLOT(slotError(QString)));
}

convertertest::AlkQuoteReceiver::~AlkQuoteReceiver()
{
}

void convertertest::AlkQuoteReceiver::slotGetQuote(const QString &a, const QString &b,
                                                   const QDate &d, const double &m)
{
    if (m_verbose) {
        qDebug() << "test::AlkQuoteReceiver::slotGetQuote( , " << a << " , " << b << " , " << d
                 << " , " << m << " )";
    }
    m_price = AlkValue(m);
    m_date = d;
}

void convertertest::AlkQuoteReceiver::slotStatus(const QString &msg)
{
    if (m_verbose) {
        qDebug() << "test::AlkQuoteReceiver::slotStatus( " << msg << " )";
    }

    m_statuses += msg;
}

void convertertest::AlkQuoteReceiver::slotError(const QString &msg)
{
    if (m_verbose) {
        qDebug() << "test::AlkQuoteReceiver::slotError( " << msg << " )";
    }

    m_errors += msg;
}
