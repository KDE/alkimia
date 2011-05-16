/***************************************************************************
 *   Copyright 2011  Alvaro Soliverez  asoliverez@kde.org                  *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
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

#include "alkquoteitem.h"

class AlkQuoteItem::Private
{
public:
  QString m_symbol;
  QDateTime m_dateTime;
  AlkValue m_openingValue;
  AlkValue m_highValue;
  AlkValue m_lowValue;
  AlkValue m_closingValue;
  AlkValue m_volume;
  AlkValue m_marketCap;
  QString m_id;
 
};

AlkQuoteItem::AlkQuoteItem(QObject *parent) :
  QObject(parent),
  d(new Private)
{
}

AlkQuoteItem::~AlkQuoteItem()
{
}

AlkQuoteItem::AlkQuoteItem(const AlkQuoteItem& item, QObject* parent):
  QObject(parent),
  d(new Private)
{
  setSymbol(item.symbol());
  setDateTime(item.dateTime());
  setOpeningValue(item.openingValue());
  setHighValue(item.highValue());
  setLowValue(item.lowValue());
  setClosingValue(item.closingValue());
  setVolume(item.volume());
  setMarketCap(item.marketCap());
  setRecordId(item.recordId());
}

const QString& AlkQuoteItem::symbol() const
{
  return d->m_symbol;
}

const QDateTime& AlkQuoteItem::dateTime() const
{
  return d->m_dateTime;
}

const AlkValue& AlkQuoteItem::openingValue() const
{
  return d->m_openingValue;
}

const AlkValue& AlkQuoteItem::highValue() const
{
  return d->m_highValue;
}

const AlkValue& AlkQuoteItem::lowValue() const
{
  return d->m_lowValue;
}

const AlkValue& AlkQuoteItem::closingValue() const
{
  return d->m_closingValue;
}

const AlkValue& AlkQuoteItem::volume() const
{
  return d->m_volume;
}

const AlkValue& AlkQuoteItem::marketCap() const
{
  return d->m_marketCap;
}

const QString& AlkQuoteItem::recordId() const
{
  return d->m_id;
}

void AlkQuoteItem::setSymbol(const QString& symbol)
{
  d->m_symbol = symbol;
}

void AlkQuoteItem::setDateTime(const QDateTime& dateTime)
{
  d->m_dateTime = dateTime;
}

void AlkQuoteItem::setOpeningValue(const AlkValue& value)
{
  d->m_openingValue = value;
}

void AlkQuoteItem::setHighValue(const AlkValue& value)
{
  d->m_highValue = value;
}

void AlkQuoteItem::setLowValue(const AlkValue& value)
{
  d->m_lowValue = value;
}

void AlkQuoteItem::setClosingValue(const AlkValue& value)
{
  d->m_closingValue = value;
}

void AlkQuoteItem::setMarketCap(const AlkValue& value)
{
  d->m_marketCap = value;
}

void AlkQuoteItem::setVolume(const AlkValue& value)
{
  d->m_volume = value;
}

void AlkQuoteItem::setRecordId(const QString& recordId)
{
  d->m_id = recordId;
}


QDBusArgument& operator<<(QDBusArgument& argument, const AlkQuoteItem &item)
{
  argument.beginStructure();
  argument << item.symbol() << item.dateTime().toString(Qt::ISODate) << item.openingValue().toString()
      << item.highValue().toString() << item.lowValue().toString() << item.closingValue().toString()
      << item.marketCap().toString() << item.volume().toString();
  argument.endStructure();
  return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, AlkQuoteItem &item)
{
  argument.beginStructure();
  QString symbol;
  QString dateTime;
  QString openingValue;
  QString highValue;
  QString lowValue;
  QString closingValue;
  QString marketCap;
  QString volume;
  
  argument >> symbol >> dateTime >> openingValue >> highValue >> lowValue >> closingValue >> marketCap >> volume;
  item.setSymbol(symbol);
  item.setDateTime(QDateTime::fromString(dateTime, Qt::ISODate));
  item.setOpeningValue(AlkValue(openingValue, '.'));
  item.setHighValue(AlkValue(highValue, '.'));
  item.setLowValue(AlkValue(lowValue, '.'));
  item.setClosingValue(AlkValue(closingValue, '.'));
  item.setMarketCap(AlkValue(marketCap, '.'));
  item.setVolume(AlkValue(volume, '.'));
  
  argument.endStructure();
  return argument;
}

