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

#include "alkimia/alkvalue.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkValueTest)

namespace QTest {
template<>
char *toString(const AlkValue &val)
{
    return qstrdup(val.toString().toUtf8());
}
}

void AlkValueTest::init()
{
}

void AlkValueTest::cleanup()
{
}

void AlkValueTest::emptyCtor()
{
    AlkValue *m = new AlkValue();
    QCOMPARE(m->toString(), QLatin1String("0/1"));
    delete m;
}

void AlkValueTest::copyCtor()
{
    AlkValue a(41, 152);
    AlkValue b(a);

    QCOMPARE(a.toString(), QLatin1String("41/152"));
    QCOMPARE(b.toString(), QLatin1String("41/152"));
}

void AlkValueTest::intCtor()
{
    AlkValue *m;
    m = new AlkValue(10);
    QCOMPARE(m->toString(), QLatin1String("10/1"));
    delete m;

    m = new AlkValue(-10);
    QCOMPARE(m->toString(), QLatin1String("-10/1"));
    delete m;

    m = new AlkValue(10, 2);
    QCOMPARE(m->toString(), QLatin1String("5/1"));
    delete m;

    m = new AlkValue(-10, 2);
    QCOMPARE(m->toString(), QLatin1String("-5/1"));
    delete m;

    // the denominator is an unsigned value thus sticking in a negative
    // one does not make sense
    m = new AlkValue(-10, -2);
    QVERIFY(m->toString() != QLatin1String("-5/1"));
    delete m;
}

void AlkValueTest::stringCtor()
{
    AlkValue *m;

    // mixed mode used for some price information in QIF
    m = new AlkValue(QLatin1String("5 8/16"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(5.5));
    delete m;

    // standard representation
    m = new AlkValue(QLatin1String("12345/100"), QLatin1Char('.'));
    QCOMPARE(m->toString(), QLatin1String("2469/20"));
    delete m;

    // negative standard representation
    m = new AlkValue(QLatin1String("-8/32"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-0.25));
    delete m;

    // false negative standard representation
    m = new AlkValue(QLatin1String("(8/32)"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-832));
    delete m;

    // duplicate negative standard representation
    m = new AlkValue(QLatin1String("(-8/32)"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-832));
    delete m;

    // duplicate negative standard representation
    m = new AlkValue(QLatin1String("-(8/32)"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-832));
    delete m;

    // different decimal symbol
    m = new AlkValue(QLatin1String("x1.234,568 EUR"), QLatin1Char(','));
    QCOMPARE(m->toString(), QLatin1String("154321/125"));
    delete m;

    // octal leadin
    m = new AlkValue(QLatin1String("0.07"), QLatin1Char('.'));
    QCOMPARE(m->toString(), QLatin1String("7/100"));
    delete m;
    m = new AlkValue(QLatin1String("0.09"), QLatin1Char('.'));
    QCOMPARE(m->toString(), QLatin1String("9/100"));
    delete m;
    m = new AlkValue(QLatin1String("09"), QLatin1Char('.'));
    QCOMPARE(m->toString(), QLatin1String("9/1"));
    delete m;

    // negative numbers
    m = new AlkValue(QLatin1String("x(1,234.)"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-1234));
    delete m;
    m = new AlkValue(QLatin1String("x-1,234."), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-1234));
    delete m;
    m = new AlkValue(QLatin1String("x1,234.-"), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue(-1234));
    delete m;

    // empty string
    m = new AlkValue(QLatin1String(""), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue());
    delete m;
    m = new AlkValue(QLatin1String("."), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue());
    delete m;
    m = new AlkValue(QLatin1String(","), QLatin1Char('.'));
    QCOMPARE(*m, AlkValue());
    delete m;
    m = new AlkValue(QLatin1String("."), QLatin1Char(','));
    QCOMPARE(*m, AlkValue());
    delete m;
}

void AlkValueTest::doubleCtor()
{
    for (int i = -123456; i < 123456; ++i) {
        double d = i;
        AlkValue r(i, 100);
        d /= 100;
        AlkValue t(d, 100);
        QCOMPARE(t, r);
    }

    AlkValue a = AlkValue(1.9999999999998);
    QVERIFY(a != AlkValue(2, 1));
    QVERIFY(a < AlkValue(2, 1));
    QVERIFY(a > AlkValue(QLatin1String("1.999999999"), QLatin1Char('.')));

    a = AlkValue(1.9999999999998, 100);
    QCOMPARE(a, AlkValue(2, 1));

    a = AlkValue(1.234, AlkValue::precisionToDenominator(2).get_ui());
    QCOMPARE(a, AlkValue(123, 100));
}

void AlkValueTest::assignment()
{
    // const AlkValue& operator=(const AlkValue& val);
    AlkValue m0;
    AlkValue m1(10, 2);
    m0 = m1;
    QCOMPARE(m0.toString(), QLatin1String("5/1"));

    // const AlkValue& operator=(int num);
    m0 = 6;
    QCOMPARE(m0.toString(), QLatin1String("6/1"));
    m0 = -12;
    QCOMPARE(m0.toString(), QLatin1String("-12/1"));

    // const AlkValue& operator=(double num);
    m0 = 123.45;
    QCOMPARE(m0.toString(), QLatin1String("8687021468732621/70368744177664"));
    m0 = 1.23e7;
    QCOMPARE(m0.toString(), QLatin1String("12300000/1"));

    // const AlkValue& operator=(const QString& str);
    m0 = QLatin1String("x1234.567");
    QCOMPARE(m0.toString(), QLatin1String("1234567/1000"));
    m0 = QLatin1String("(x1234.567)");
    QCOMPARE(m0.toString(), QLatin1String("-1234567/1000"));
    m0 = QLatin1String("-1234.567");
    QCOMPARE(m0.toString(), QLatin1String("-1234567/1000"));
}

void AlkValueTest::equality()
{
    AlkValue m0, m1;
    m0 = 123;
    m1 = QLatin1String("123");
    QCOMPARE(m0, m1);

    m0 = QLatin1String("511/100");
    m1 = QLatin1String("5753348523965686/1125899906842600");
    QCOMPARE(m0, m1);

    m0 = QLatin1String("-14279570/100");
    m1 = QLatin1String("-1427957/10");
    QCOMPARE(m0, m1);

    m0 = QLatin1String("-7301028/100");
    m1 = QLatin1String("-1825257/25");
    QCOMPARE(m0, m1);
}

void AlkValueTest::inequality()
{
    AlkValue m0, m1;
    m0 = 123;
    m1 = QLatin1String("124");
    QVERIFY(m0 != m1);

    m0 = QLatin1String("511/100");
    m1 = QLatin1String("5753348523965809/1125899906842624");
    QVERIFY(m0 != m1);
}

void AlkValueTest::less()
{
    AlkValue m0, m1;
    m0 = 12;
    m1 = 13;
    QVERIFY(m0 < m1);
    m0 = -m0;
    m1 = -m1;
    QVERIFY(m1 < m0);

    m0 = 12;
    m1 = AlkValue(QLatin1String("12.0000000000000000000000000000001"), QLatin1Char('.'));
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

void AlkValueTest::greater()
{
    AlkValue m0, m1;
    m0 = 12;
    m1 = 13;
    QVERIFY(m1 > m0);
    m0 = -m0;
    m1 = -m1;
    QVERIFY(m0 > m1);

    m0 = 12;
    m1 = AlkValue(QLatin1String("12.0000000000000000000000000000001"), QLatin1Char('.'));
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

void AlkValueTest::lessThan()
{
    AlkValue m0, m2;
    AlkValue m1 = AlkValue(QLatin1String("12.0000000000000000000000000000001"), QLatin1Char('.'));
    m0 = 12;
    m2 = 12;
    QVERIFY(m0 <= m1);
    QVERIFY(m0 <= m2);
    m0 = -m0;
    m1 = -m1;
    m2 = -m2;
    QVERIFY(m1 <= m0);
    QVERIFY(m2 <= m0);
}

void AlkValueTest::greaterThan()
{
    AlkValue m0, m2;
    AlkValue m1 = AlkValue(QLatin1String("12.0000000000000000000000000000001"), QLatin1Char('.'));
    m0 = 12;
    m2 = 12;
    QVERIFY(m1 >= m0);
    QVERIFY(m2 >= m0);
    m0 = -m0;
    m1 = -m1;
    m2 = -m2;
    QVERIFY(m0 >= m1);
    QVERIFY(m0 >= m2);
}

void AlkValueTest::addition()
{
    // AlkValue operator+( const AlkValue& summand ) const;
    AlkValue m0, m1;
    m0 = 100;
    m1 = 23;
    QCOMPARE((m0 + m1), AlkValue(123));

    // AlkValue& operator+= ( const AlkValue&  val );
    m0 += m1;
    QCOMPARE(m0, AlkValue(123));

    m0 = 100;
    m1 = -23;
    QCOMPARE((m0 + m1), AlkValue(77));

    m0 += m1;
    QCOMPARE(m0, AlkValue(77));
}

void AlkValueTest::subtraction()
{
    // AlkValue operator-( const AlkValue& minuend ) const;
    AlkValue m0, m1;
    m0 = 100;
    m1 = 23;
    QCOMPARE((m0 - m1), AlkValue(77));

    // AlkValue& operator-= ( const AlkValue&  val );
    m0 -= m1;
    QCOMPARE(m0, AlkValue(77));

    m0 = 100;
    m1 = -23;
    QCOMPARE((m0 - m1), AlkValue(123));

    m0 -= m1;
    QCOMPARE(m0, AlkValue(123));
}

void AlkValueTest::multiplication()
{
    // AlkValue operator*( const AlkValue& factor ) const;
    AlkValue m0, m1;
    m0 = 100;
    m1 = 23;
    QCOMPARE((m0 * m1), AlkValue(2300));

    // AlkValue& operator*= ( const AlkValue&  val );
    m0 *= m1;
    QCOMPARE(m0, AlkValue(2300));

    m0 = 100;
    m1 = -23;
    QCOMPARE((m0 * m1), AlkValue(-2300));

    m0 *= m1;
    QCOMPARE(m0, AlkValue(-2300));

    // AlkValue operator*( int factor) const;
    QCOMPARE((m1 * 4), AlkValue(-92));
    QCOMPARE((m1 *(-4)), AlkValue(92));
}

void AlkValueTest::division()
{
    // AlkValue operator/( const AlkValue& divisor ) const;
    AlkValue m0, m1;
    m0 = 100;
    m1 = 20;
    QCOMPARE((m0 / m1), AlkValue(5));

    // AlkValue& operator/= ( const AlkValue&  val );
    m0 /= m1;
    QCOMPARE(m0, AlkValue(5));

    m0 = 100;
    m1 = -20;
    QCOMPARE((m0 / m1), AlkValue(-5));

    m0 /= m1;
    QCOMPARE(m0, AlkValue(-5));
}

void AlkValueTest::modulo()
{
    AlkValue m0(1025000), m1;
    m1 = m0 % 97;
    QCOMPARE(m1.abs(), AlkValue(1));

    m0 = 1024999;
    m1 = m0 % 97;
    QCOMPARE(m1.abs(), AlkValue(0));
}

void AlkValueTest::unaryMinus()
{
    // AlkValue operator-() const;
    AlkValue m0(5);
    QCOMPARE(-m0, AlkValue(-5));
}

void AlkValueTest::abs()
{
    AlkValue m0(-5);
    AlkValue m1(5);
    QCOMPARE(m0.abs(), AlkValue(5));
    QCOMPARE(m1.abs(), AlkValue(5));
}

void AlkValueTest::precision()
{
    AlkValue a(QLatin1String("1234567890"), QLatin1Char('.'));
    AlkValue b(QLatin1String("1234567890"), QLatin1Char('.'));
    AlkValue c;

    // QVERIFY(c.isZero() == true);
    c = a * b;
    QCOMPARE(c, AlkValue(QLatin1String("1524157875019052100"), QLatin1Char('.')));
    c /= b;
    QCOMPARE(c, AlkValue(QLatin1String("1234567890"), QLatin1Char('.')));
}

void AlkValueTest::convertDenominator()
{
    AlkValue a(123.456);
    QCOMPARE(a.convertDenominator(), AlkValue(12346, 100));

    AlkValue b;
    a = QLatin1String("-73010.28");
    b = QLatin1String("1.95583");
    QCOMPARE((a * b).convertDenominator(100), AlkValue(-14279570, 100));

    a = QLatin1String("-142795.69");
    QCOMPARE((a / b).convertDenominator(100), AlkValue(-7301028, 100));

    a = QLatin1String("142795.69");
    QCOMPARE((a / b).convertDenominator(100), AlkValue(7301028, 100));

    a = AlkValue(1.9999999999998);
    QVERIFY(a != AlkValue(2, 1));
    QVERIFY(a < AlkValue(2, 1));
    QVERIFY(a > AlkValue(QLatin1String("1.999999999"), QLatin1Char('.')));

    a = AlkValue(1.9999999999998, 100);
    QCOMPARE(a, AlkValue(2, 1));
}

void AlkValueTest::convertPrecision()
{
    AlkValue a(123.456);
    QCOMPARE(a.convertPrecision(), AlkValue(12346, 100));

    AlkValue b;
    a = QLatin1String("-73010.28");
    b = QLatin1String("1.95583");
    QCOMPARE((a * b).convertPrecision(2), AlkValue(-14279570, 100));

    a = QLatin1String("-142795.69");
    QCOMPARE((a / b).convertPrecision(2), AlkValue(-7301028, 100));

    a = QLatin1String("142795.69");
    QCOMPARE((a / b).convertPrecision(2), AlkValue(7301028, 100));

    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundFloor), AlkValue());
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundFloor), AlkValue(-1));
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundFloor), AlkValue(1));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundFloor), AlkValue(-2));

    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundCeil), AlkValue(1));
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundCeil), AlkValue());
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundCeil), AlkValue(2));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundCeil), AlkValue(-1));

    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundTruncate), AlkValue());
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundTruncate), AlkValue());
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundTruncate), AlkValue(1));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundTruncate), AlkValue(-1));

    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundPromote), AlkValue(1));
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundPromote), AlkValue(-1));
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundPromote), AlkValue(2));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundPromote), AlkValue(-2));

    QCOMPARE(AlkValue(4, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue());
    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue());
    QCOMPARE(AlkValue(6, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(1));
    QCOMPARE(AlkValue(-4, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue());
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue());
    QCOMPARE(AlkValue(-6, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(-1));
    QCOMPARE(AlkValue(14, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(1));
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(1));
    QCOMPARE(AlkValue(16, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(2));
    QCOMPARE(AlkValue(-14, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(-1));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(-1));
    QCOMPARE(AlkValue(-16, 10).convertPrecision(0, AlkValue::RoundHalfDown), AlkValue(-2));

    QCOMPARE(AlkValue(4, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue());
    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(1));
    QCOMPARE(AlkValue(6, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(1));
    QCOMPARE(AlkValue(-4, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue());
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(-1));
    QCOMPARE(AlkValue(-6, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(-1));
    QCOMPARE(AlkValue(14, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(1));
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(2));
    QCOMPARE(AlkValue(16, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(2));
    QCOMPARE(AlkValue(-14, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(-1));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(-2));
    QCOMPARE(AlkValue(-16, 10).convertPrecision(0, AlkValue::RoundHalfUp), AlkValue(-2));

    QCOMPARE(AlkValue(5, 10).convertPrecision(0, AlkValue::RoundRound), AlkValue());
    QCOMPARE(AlkValue(-5, 10).convertPrecision(0, AlkValue::RoundRound), AlkValue());
    QCOMPARE(AlkValue(15, 10).convertPrecision(0, AlkValue::RoundRound), AlkValue(2));
    QCOMPARE(AlkValue(-15, 10).convertPrecision(0, AlkValue::RoundRound), AlkValue(-2));
    QCOMPARE(AlkValue(25, 10).convertPrecision(0, AlkValue::RoundRound), AlkValue(2));
    QCOMPARE(AlkValue(-25, 10).convertPrecision(0, AlkValue::RoundRound), AlkValue(-2));
}

void AlkValueTest::denominatorToPrecision()
{
    QVERIFY(AlkValue::denominatorToPrecision(100) == 2);
    QVERIFY(AlkValue::denominatorToPrecision(1000000) == 6);
    QVERIFY(AlkValue::denominatorToPrecision(1) == 0);
    QVERIFY(AlkValue::denominatorToPrecision(0) == 0);
    QVERIFY(AlkValue::denominatorToPrecision(-1) == 0);
    QVERIFY(AlkValue::denominatorToPrecision(-188) == 0);
    QVERIFY(AlkValue::denominatorToPrecision(200) == 3);
}

void AlkValueTest::precisionToDenominator()
{
    QVERIFY(AlkValue::precisionToDenominator(2) == 100);
    QVERIFY(AlkValue::precisionToDenominator(6) == 1000000);
    QVERIFY(AlkValue::precisionToDenominator(0) == 1);
    QVERIFY(AlkValue::precisionToDenominator(-1) == 1);
    QVERIFY(AlkValue::precisionToDenominator(-5) == 1);
}

void AlkValueTest::valueRef()
{
    AlkValue a(5);

    mpq_class &val = a.valueRef();
    val = mpq_class(1, 3);

    QCOMPARE(a, AlkValue(1, 3));

    a = QLatin1String("1/7");

    QCOMPARE(val, mpq_class(1, 7));
}

void AlkValueTest::canonicalize()
{
    AlkValue a(5);
    mpq_class &val(a.valueRef());
    QCOMPARE(val, mpq_class(5, 1));

    mpz_class i;
    i = 10;
    mpq_set_num(val.get_mpq_t(), i.get_mpz_t());
    i = 2;
    mpq_set_den(val.get_mpq_t(), i.get_mpz_t());
    QVERIFY(val != mpq_class(5, 1));
    QCOMPARE(val, mpq_class(10, 2));

    a.canonicalize();
    QCOMPARE(val, mpq_class(5, 1));
}
