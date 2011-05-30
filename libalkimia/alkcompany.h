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

#ifndef ALKCOMPANY_H
#define ALKCOMPANY_H

#include "alk_export.h"


#include <QMetaType>
#include <QDBusArgument>
#include <QObject>
#include <QString>

class ALK_EXPORT AlkCompany : public QObject
{
  Q_OBJECT
public:
  AlkCompany(QObject *parent = 0);
  AlkCompany(const AlkCompany &company, QObject *parent = 0);
  ~AlkCompany();
  
  /**
   * Financial symbol
   */
  const QString& symbol() const;
  
  /**
   * Name of the equity
   */
  const QString& name() const;
  
  /**
   * Type of the equity
   */
  const QString& type() const;
  
  /**
   * Exchange market of the equity
   */
  const QString& exchange() const;
  
  /**
   * Internal id
   */
  const QString& recordId() const;
  
  void setSymbol(const QString &symbol);
  void setName(const QString &name);
  void setType(const QString &type);
  void setExchange(const QString &exchange);
  void setRecordId(const QString &recordId);
  
  
private:
    /// \internal d-pointer class.
  class Private;
  /// \internal d-pointer instance.
  Private* const d;
 
  
};

ALK_EXPORT QDBusArgument& operator<<(QDBusArgument& argument, const AlkCompany &company);
ALK_EXPORT const QDBusArgument& operator>>(const QDBusArgument& argument, AlkCompany &company);

Q_DECLARE_METATYPE(AlkCompany)

#endif
