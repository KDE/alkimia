/*
    SPDX-FileCopyrightText: 2011 Alvaro Soliverez asoliverez @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkquoteitem.h"

class AlkQuoteItem::Private
{
public:
    QString m_symbol;
    QDateTime m_dateTime;
    AlkValue m_currentValue;
    AlkValue m_openingValue;
    AlkValue m_highValue;
    AlkValue m_lowValue;
    AlkValue m_closingValue;
    AlkValue m_volume;
    AlkValue m_marketCap;
    AlkValue m_changeToday;
    AlkValue m_earnings;
    AlkValue m_ebitda;
    QString m_id;
};

AlkQuoteItem::AlkQuoteItem(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

AlkQuoteItem::~AlkQuoteItem()
{
}

AlkQuoteItem::AlkQuoteItem(const AlkQuoteItem &item, QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    setSymbol(item.symbol());
    setDateTime(item.dateTime());
    setCurrentValue(item.currentValue());
    setOpeningValue(item.openingValue());
    setHighValue(item.highValue());
    setLowValue(item.lowValue());
    setClosingValue(item.closingValue());
    setVolume(item.volume());
    setMarketCap(item.marketCap());
    setEarningsPerShare(item.earningsPerShare());
    setChangeToday(item.changeToday());
    setEbitda(item.ebitda());
    setRecordId(item.recordId());
}

const QString &AlkQuoteItem::symbol() const
{
    return d->m_symbol;
}

const QDateTime &AlkQuoteItem::dateTime() const
{
    return d->m_dateTime;
}

const AlkValue &AlkQuoteItem::currentValue() const
{
    return d->m_currentValue;
}

const AlkValue &AlkQuoteItem::openingValue() const
{
    return d->m_openingValue;
}

const AlkValue &AlkQuoteItem::highValue() const
{
    return d->m_highValue;
}

const AlkValue &AlkQuoteItem::lowValue() const
{
    return d->m_lowValue;
}

const AlkValue &AlkQuoteItem::closingValue() const
{
    return d->m_closingValue;
}

const AlkValue &AlkQuoteItem::volume() const
{
    return d->m_volume;
}

const AlkValue &AlkQuoteItem::marketCap() const
{
    return d->m_marketCap;
}

const AlkValue &AlkQuoteItem::earningsPerShare() const
{
    return d->m_earnings;
}

const AlkValue &AlkQuoteItem::changeToday() const
{
    return d->m_changeToday;
}

const AlkValue &AlkQuoteItem::ebitda() const
{
    return d->m_ebitda;
}

const QString &AlkQuoteItem::recordId() const
{
    return d->m_id;
}

void AlkQuoteItem::setSymbol(const QString &symbol)
{
    d->m_symbol = symbol;
}

void AlkQuoteItem::setDateTime(const QDateTime &dateTime)
{
    d->m_dateTime = dateTime;
}

void AlkQuoteItem::setCurrentValue(const AlkValue &value)
{
    d->m_currentValue = value;
}

void AlkQuoteItem::setOpeningValue(const AlkValue &value)
{
    d->m_openingValue = value;
}

void AlkQuoteItem::setHighValue(const AlkValue &value)
{
    d->m_highValue = value;
}

void AlkQuoteItem::setLowValue(const AlkValue &value)
{
    d->m_lowValue = value;
}

void AlkQuoteItem::setClosingValue(const AlkValue &value)
{
    d->m_closingValue = value;
}

void AlkQuoteItem::setMarketCap(const AlkValue &value)
{
    d->m_marketCap = value;
}

void AlkQuoteItem::setVolume(const AlkValue &value)
{
    d->m_volume = value;
}

void AlkQuoteItem::setEarningsPerShare(const AlkValue &value)
{
    d->m_earnings = value;
}

void AlkQuoteItem::setChangeToday(const AlkValue &value)
{
    d->m_changeToday = value;
}

void AlkQuoteItem::setEbitda(const AlkValue &value)
{
    d->m_ebitda = value;
}

void AlkQuoteItem::setRecordId(const QString &recordId)
{
    d->m_id = recordId;
}

QDBusArgument &operator<<(QDBusArgument &argument, const AlkQuoteItem &item)
{
    argument.beginStructure();
    argument << item.symbol() << item.dateTime().toString(Qt::ISODate)
             << item.currentValue().toString()
             << item.openingValue().toString()
             << item.highValue().toString() << item.lowValue().toString()
             << item.closingValue().toString()
             << item.marketCap().toString() << item.volume().toString()
             << item.earningsPerShare().toString()
             << item.changeToday().toString() << item.ebitda().toString() << item.recordId();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, AlkQuoteItem &item)
{
    argument.beginStructure();
    QString symbol;
    QString dateTime;
    QString currentValue;
    QString openingValue;
    QString highValue;
    QString lowValue;
    QString closingValue;
    QString marketCap;
    QString volume;
    QString earnings;
    QString change;
    QString ebitda;
    QString recordId;

    argument >> symbol >> dateTime >> currentValue >> openingValue >> highValue >> lowValue
    >> closingValue >> marketCap >> volume >> earnings >> change >> ebitda >> recordId;
    item.setSymbol(symbol);
    item.setDateTime(QDateTime::fromString(dateTime, Qt::ISODate));
    item.setCurrentValue(AlkValue(currentValue, '.'));
    item.setOpeningValue(AlkValue(openingValue, '.'));
    item.setHighValue(AlkValue(highValue, '.'));
    item.setLowValue(AlkValue(lowValue, '.'));
    item.setClosingValue(AlkValue(closingValue, '.'));
    item.setMarketCap(AlkValue(marketCap, '.'));
    item.setVolume(AlkValue(volume, '.'));
    item.setEarningsPerShare(AlkValue(earnings, '.'));
    item.setChangeToday(AlkValue(change, '.'));
    item.setEbitda(AlkValue(ebitda, '.'));
    item.setRecordId(recordId);

    argument.endStructure();
    return argument;
}
