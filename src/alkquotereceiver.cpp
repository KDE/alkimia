/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkquotereceiver.h"

#include "alkdebug.h"
#include "alkonlinequote.h"

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
        alkDebug() << "test::AlkQuoteReceiver::slotGetQuote( , " << a << " , " << b << " , " << d
                 << " , " << m << " )";
    }
    m_price = AlkValue(m);
    m_date = d;
}

void convertertest::AlkQuoteReceiver::slotStatus(const QString &msg)
{
    if (m_verbose) {
        alkDebug() << "test::AlkQuoteReceiver::slotStatus( " << msg << " )";
    }

    m_statuses += msg;
}

void convertertest::AlkQuoteReceiver::slotError(const QString &msg)
{
    if (m_verbose) {
        alkDebug() << "test::AlkQuoteReceiver::slotError( " << msg << " )";
    }

    m_errors += msg;
}
