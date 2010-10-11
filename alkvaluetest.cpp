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

#include "alkvaluetest.h"
#include "alkvalue.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkValueTest)

void AlkValueTest::init(void)
{
}

void AlkValueTest::cleanup(void)
{
}

void AlkValueTest::emptyCtor(void)
{
  AlkValue *m = new AlkValue();
  QVERIFY(m->toString() == QLatin1String("0/1"));
  delete m;
}


void AlkValueTest::intCtor(void)
{
  AlkValue *m;
  m = new AlkValue(10);
  QVERIFY(m->toString() == QLatin1String("10/1"));
  delete m;

  m = new AlkValue(-10);
  QVERIFY(m->toString() == QLatin1String("-10/1"));
  delete m;

  m = new AlkValue(10, 2);
  QVERIFY(m->toString() == QLatin1String("5/1"));
  delete m;

  m = new AlkValue(-10, 2);
  QVERIFY(m->toString() == QLatin1String("-5/1"));
  delete m;

  // the denominator is an unsigned value thus sticking in a negative
  // one does not make sense
  m = new AlkValue(-10, -2);
  QVERIFY(m->toString() != QLatin1String("-5/1"));
  delete m;
}

void AlkValueTest::stringCtor(void)
{
  AlkValue *m;

  // mixed mode used for some price information in QIF
  m = new AlkValue("5 8/16", '.');
  QVERIFY(*m == AlkValue(5.5));
  delete m;

  // standard representation
  m = new AlkValue("12345/100", '.');
  QVERIFY(m->toString() == QLatin1String("2469/20"));
  delete m;

  // negative standard representation
  m = new AlkValue("-8/32", '.');
  QVERIFY(*m == AlkValue(-0.25));
  delete m;

  // false negative standard representation
  m = new AlkValue("(8/32)", '.');
  QVERIFY(*m == AlkValue(-832));
  delete m;

  // duplicate negative standard representation
  m = new AlkValue("(-8/32)", '.');
  QVERIFY(*m == AlkValue(-832));
  delete m;

  // duplicate negative standard representation
  m = new AlkValue("-(8/32)", '.');
  QVERIFY(*m == AlkValue(-832));
  delete m;

  // different decimal symbol
  m = new AlkValue("x1.234,568 EUR", ',');
  QVERIFY(m->toString() == QLatin1String("154321/125"));
  delete m;

  // octal leadin
  m = new AlkValue("0.07", '.');
  QVERIFY(m->toString() == QLatin1String("7/100"));
  delete m;
  m = new AlkValue("0.09", '.');
  QVERIFY(m->toString() == QLatin1String("9/100"));
  delete m;

  // negative numbers
  m = new AlkValue("x(1,234.)", '.');
  QVERIFY(*m == AlkValue(-1234));
  delete m;
  m = new AlkValue("x-1,234.", '.');
  QVERIFY(*m == AlkValue(-1234));
  delete m;
  m = new AlkValue("x1,234.-", '.');
  QVERIFY(*m == AlkValue(-1234));
  delete m;

  // empty string
  m = new AlkValue("", '.');
  QVERIFY(*m == AlkValue());
  delete m;
  m = new AlkValue(".", '.');
  QVERIFY(*m == AlkValue());
  delete m;
  m = new AlkValue(",", '.');
  QVERIFY(*m == AlkValue());
  delete m;
  m = new AlkValue(".", ',');
  QVERIFY(*m == AlkValue());
  delete m;
}

void AlkValueTest::assignment(void)
{
  // const AlkValue& operator=(const AlkValue& val);
  AlkValue m0;
  AlkValue m1(10, 2);
  m0 = m1;
  QVERIFY(m0.toString() == QLatin1String("5/1"));

  // const AlkValue& operator=(int num);
  m0 = 6;
  QVERIFY(m0.toString() == QLatin1String("6/1"));
  m0 = -12;
  QVERIFY(m0.toString() == QLatin1String("-12/1"));

  // const AlkValue& operator=(double num);
  m0 = 123.45;
  QVERIFY(m0.toString() == QLatin1String("8687021468732621/70368744177664"));
  m0 = 1.23e7;
  QVERIFY(m0.toString() == QLatin1String("12300000/1"));

  // const AlkValue& operator=(const QString& str);
  m0 = QLatin1String("x1234.567");
  QVERIFY(m0.toString() == QLatin1String("1234567/1000"));
  m0 = QLatin1String("(x1234.567)");
  QVERIFY(m0.toString() == QLatin1String("-1234567/1000"));
  m0 = QLatin1String("-1234.567");
  QVERIFY(m0.toString() == QLatin1String("-1234567/1000"));
}

void AlkValueTest::equality(void)
{
  AlkValue m0, m1;
  m0 = 123;
  m1 = QLatin1String("123");
  QVERIFY(m0 == m1);
}

void AlkValueTest::inequality(void)
{
  AlkValue m0, m1;
  m0 = 123;
  m1 = QLatin1String("124");
  QVERIFY(m0 != m1);
}

void AlkValueTest::less(void)
{
  AlkValue m0 = 12;
  AlkValue m1 = 13;
  QVERIFY(m0 < m1);
  m0 = -m0;
  m1 = -m1;
  QVERIFY(m1 < m0);

  m0 = 12;
  m1 = AlkValue("12.0000000000000000000000000000001", '.');
  QVERIFY(m0 < m1);
  QVERIFY(!(m1 < m0));
  m0 = -m0;
  m1 = -m1;
  QVERIFY(m1 < m0);
  QVERIFY(!(m0 < m1));

  m0 = 12;
  m1 = m0;
  QVERIFY(!(m0 < m1));
}

void AlkValueTest::greater(void)
{
  AlkValue m0 = 12;
  AlkValue m1 = 13;
  QVERIFY(m1 > m0);
  m0 = -m0;
  m1 = -m1;
  QVERIFY(m0 > m1);

  m0 = 12;
  m1 = AlkValue("12.0000000000000000000000000000001", '.');
  QVERIFY(m1 > m0);
  QVERIFY(!(m0 > m1));
  m0 = -m0;
  m1 = -m1;
  QVERIFY(m0 > m1);
  QVERIFY(!(m1 > m0));

  m0 = 12;
  m1 = m0;
  QVERIFY(!(m0 > m1));
}

void AlkValueTest::lessThan(void)
{
  AlkValue m0 = 12;
  AlkValue m1 = AlkValue("12.0000000000000000000000000000001", '.');
  AlkValue m2 = 12;
  QVERIFY(m0 <= m1);
  QVERIFY(m0 <= m2);
  m0 = -m0;
  m1 = -m1;
  m2 = -m2;
  QVERIFY(m1 <= m0);
  QVERIFY(m2 <= m0);
}

void AlkValueTest::greaterThan(void)
{
  AlkValue m0 = 12;
  AlkValue m1 = AlkValue("12.0000000000000000000000000000001", '.');
  AlkValue m2 = 12;
  QVERIFY(m1 >= m0);
  QVERIFY(m2 >= m0);
  m0 = -m0;
  m1 = -m1;
  m2 = -m2;
  QVERIFY(m0 >= m1);
  QVERIFY(m0 >= m2);
}

void AlkValueTest::addition(void)
{
  // AlkValue operator+( const AlkValue& summand ) const;
  AlkValue m0, m1;
  m0 = 100;
  m1 = 23;
  QVERIFY((m0 + m1) == AlkValue(123));

  // AlkValue& operator+= ( const AlkValue&  val );
  m0 += m1;
  QVERIFY(m0 == AlkValue(123));

  m0 = 100;
  m1 = -23;
  QVERIFY((m0 + m1) == AlkValue(77));

  m0 += m1;
  QVERIFY(m0 == AlkValue(77));
}


void AlkValueTest::subtraction(void)
{
  // AlkValue operator-( const AlkValue& minuend ) const;
  AlkValue m0, m1;
  m0 = 100;
  m1 = 23;
  QVERIFY((m0 - m1) == AlkValue(77));

  // AlkValue& operator-= ( const AlkValue&  val );
  m0 -= m1;
  QVERIFY(m0 == AlkValue(77));

  m0 = 100;
  m1 = -23;
  QVERIFY((m0 - m1) == AlkValue(123));

  m0 -= m1;
  QVERIFY(m0 == AlkValue(123));
}

void AlkValueTest::multiplication(void)
{
  // AlkValue operator*( const AlkValue& factor ) const;
  AlkValue m0, m1;
  m0 = 100;
  m1 = 23;
  QVERIFY((m0 * m1) == AlkValue(2300));

  // AlkValue& operator*= ( const AlkValue&  val );
  m0 *= m1;
  QVERIFY(m0 == AlkValue(2300));

  m0 = 100;
  m1 = -23;
  QVERIFY((m0 * m1) == AlkValue(-2300));

  m0 *= m1;
  QVERIFY(m0 == AlkValue(-2300));

  // AlkValue operator*( int factor) const;
  QVERIFY((m1 * 4) == AlkValue(-92));
  QVERIFY((m1 *(-4)) == AlkValue(92));
}

void AlkValueTest::division(void)
{
  // AlkValue operator/( const AlkValue& divisor ) const;
  AlkValue m0, m1;
  m0 = 100;
  m1 = 20;
  QVERIFY((m0 / m1) == AlkValue(5));

  // AlkValue& operator/= ( const AlkValue&  val );
  m0 /= m1;
  QVERIFY(m0 == AlkValue(5));

  m0 = 100;
  m1 = -20;
  QVERIFY((m0 / m1) == AlkValue(-5));

  m0 /= m1;
  QVERIFY(m0 == AlkValue(-5));
}

void AlkValueTest::unaryMinus(void)
{
  // AlkValue operator-() const;
  AlkValue m0(5);
  QVERIFY(-m0 == AlkValue(-5));
}

void AlkValueTest::abs(void)
{
  AlkValue m0(-5);
  AlkValue m1(5);
  QVERIFY(m0.abs() == AlkValue(5));
  QVERIFY(m1.abs() == AlkValue(5));
}

