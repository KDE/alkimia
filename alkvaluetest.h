/***************************************************************************
 *   Copyright 2010  Thomas Baumgart  ipwizard@users.sourceforge.net       *
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

#ifndef ALKVALUETEST_H
#define ALKVALUETEST_H

#include <QtCore/QObject>

class AlkValue;

class AlkValueTest : public QObject
{
  Q_OBJECT

private slots:
  void init();
  void cleanup();
  void emptyCtor(void);
  void copyCtor(void);
  void intCtor(void);
  void stringCtor(void);
  void doubleCtor(void);
  void assignment(void);
  void equality(void);
  void inequality(void);
  void less(void);
  void greater(void);
  void lessThan(void);
  void greaterThan(void);
  void addition(void);
  void subtraction(void);
  void multiplication(void);
  void division(void);
  void unaryMinus(void);
  void abs(void);
  void precision(void);
  void convertDenom(void);
  void convertPrec(void);
  void denomToPrec(void);
  void precToDenom(void);
};

#endif // ALKVALUETEST_H

