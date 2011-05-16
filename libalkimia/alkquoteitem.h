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

#ifndef ALKQUOTEITEM_H
#define ALKQUOTEITEM_H

#include "alk_export.h"
#include "alkvalue.h"

#include <QDate>
#include <QDBusArgument>
#include <QMetaType>
#include <QTime>

class ALK_EXPORT AlkQuoteItem : public QObject
{
  Q_OBJECT

public:
  AlkQuoteItem(QObject *parent = 0);
  ~AlkQuoteItem();
  
  AlkQuoteItem(const AlkQuoteItem &item, QObject* parent = 0);
  
  const QString& symbol() const;
  const QDateTime& dateTime() const;
  const AlkValue& openingValue() const;
  const AlkValue& highValue() const;
  const AlkValue& lowValue() const;
  const AlkValue& closingValue() const;
  const AlkValue& volume() const;
  const AlkValue& marketCap() const;
  const QString& recordId() const;
  
  void setSymbol(const QString &symbol);
  void setDateTime(const QDateTime &datetime);
  void setOpeningValue(const AlkValue &value);
  void setHighValue(const AlkValue &value);
  void setLowValue(const AlkValue &value);
  void setClosingValue(const AlkValue &value);
  void setVolume(const AlkValue &value);
  void setMarketCap(const AlkValue &value);
  void setRecordId(const QString &recordId);
  
  private:
    /// \internal d-pointer class.
  class Private;
  /// \internal d-pointer instance.
  Private* const d;
  
};

ALK_EXPORT QDBusArgument& operator<<(QDBusArgument& argument, const AlkQuoteItem &item);
ALK_EXPORT const QDBusArgument& operator>>(const QDBusArgument& argument, AlkQuoteItem &item);

Q_DECLARE_METATYPE(AlkQuoteItem)

#endif
