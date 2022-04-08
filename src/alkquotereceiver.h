/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKQUOTERECEIVER_H
#define ALKQUOTERECEIVER_H

#include <alkimia/alkvalue.h>

#include <QDate>
#include <QObject>
#include <QStringList>

class AlkOnlineQuote;

namespace convertertest {
/**
Simple class to handle signals/slots for unit tests

@author Ace Jones acejones @users.sourceforge.net
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
    AlkValue m_price;
    QDate m_date;
protected:
    bool m_verbose;
};
} // end namespace convertertest

#endif // ALKQUOTERECEIVER_H
