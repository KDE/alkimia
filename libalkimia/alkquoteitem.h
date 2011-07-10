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

/**
 * This class represents a single quote of an equity
 * It holds information of use to assess the equity value
 */

class ALK_EXPORT AlkQuoteItem : public QObject
{
  Q_OBJECT

public:
  AlkQuoteItem(QObject *parent = 0);
  ~AlkQuoteItem();

  AlkQuoteItem(const AlkQuoteItem &item, QObject* parent = 0);

  /**
   * This is the symbol of the equity
   */
  const QString& symbol() const;

  /**
   * Date and time of the quote
   */
  const QDateTime& dateTime() const;

  /**
   * Value of the share at the time of the quote
   */
  const AlkValue& currentValue() const;

  /**
   * Value of the share when the market opened
   */
  const AlkValue& openingValue() const;

  /**
   * Highest value of the share since the market opened that day
   */
  const AlkValue& highValue() const;

  /**
   * Lowest value of the share since the market opened that day
   */
  const AlkValue& lowValue() const;

  /**
   * Value of the share when the market closed
   */
  const AlkValue& closingValue() const;

  /**
   * Number of shares traded
   */
  const AlkValue& volume() const;

  /**
   * Market capitalization
   */
  const AlkValue& marketCap() const;


  /**
   * Earning per share
   */
  const AlkValue& earningsPerShare() const;

  /**
   * Today's change
   */
  const AlkValue& changeToday() const;

  /**
   * Earnings Before Interest, Taxes, Depreciation, and Amortization
   */
  const AlkValue& ebitda() const;

  /**
   * Internal id
   */
  const QString& recordId() const;


  void setSymbol(const QString &symbol);
  void setDateTime(const QDateTime &datetime);
  void setCurrentValue(const AlkValue &value);
  void setOpeningValue(const AlkValue &value);
  void setHighValue(const AlkValue &value);
  void setLowValue(const AlkValue &value);
  void setClosingValue(const AlkValue &value);
  void setVolume(const AlkValue &value);
  void setMarketCap(const AlkValue &value);
  void setEarningsPerShare(const AlkValue &value);
  void setChangeToday(const AlkValue &value);
  void setEbitda(const AlkValue &value);
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
