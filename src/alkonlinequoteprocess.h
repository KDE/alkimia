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

#ifndef ALKONLINEQUOTEPROCESS_H
#define ALKONLINEQUOTEPROCESS_H

#include "alk_export.h"

#include <QMap>
#include <QStringList>

#include <kprocess.h>

/**
Helper class to attend the process which is running the script, in the case
of a local script being used to fetch the quote.

@author Thomas Baumgart <thb@net-bembel.de> & Ace Jones <acejones@users.sourceforge.net>
*/
class ALK_NO_EXPORT AlkOnlineQuoteProcess: public KProcess
{
  Q_OBJECT
public:
  AlkOnlineQuoteProcess();
  inline void setSymbol(const QString& _symbol) {
    m_symbol = _symbol; m_string.truncate(0);
  }

public slots:
  void slotReceivedDataFromFilter();
  void slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus);

signals:
  void processExited(const QString&);

private:
  QString m_symbol;
  QString m_string;
};

/**
Helper class to run the Finance::Quote process. This is used only for the purpose of obtaining
a list of valid sources. The actual price quotes are obtained thru WebPriceQuoteProcess.
The class also contains functions to convert between the rather cryptic source names used
by the Finance::Quote package, and more user-friendly names.

@author Thomas Baumgart <thb@net-bembel.de> & Ace Jones <acejones@users.sourceforge.net>, Tony B<tonybloom@users.sourceforge.net>
 */
class ALK_NO_EXPORT AlkFinanceQuoteProcess: public KProcess
{
  Q_OBJECT
public:
  AlkFinanceQuoteProcess();
  void launch(const QString& scriptPath);
  bool isFinished() const {
    return(m_isDone);
  };
  const QStringList getSourceList() const;
  const QString crypticName(const QString& niceName) const;
  const QString niceName(const QString& crypticName) const;

public slots:
  void slotReceivedDataFromFilter();
  void slotProcessExited();

private:
  bool m_isDone;
  QString m_string;
  typedef QMap<QString, QString> fqNameMap;
  fqNameMap m_fqNames;
};

#endif // ALKONLINEQUOTEPROCESS_H
