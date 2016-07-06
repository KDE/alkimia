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
  void emptyCtor();
  void copyCtor();
  void intCtor();
  void stringCtor();
  void doubleCtor();
  void assignment();
  void equality();
  void inequality();
  void less();
  void greater();
  void lessThan();
  void greaterThan();
  void addition();
  void subtraction();
  void multiplication();
  void division();
  void unaryMinus();
  void abs();
  void precision();
  void convertDenominator();
  void convertPrecision();
  void denominatorToPrecision();
  void precisionToDenominator();
  void valueRef();
  void canonicalize();
};

#endif // ALKVALUETEST_H

