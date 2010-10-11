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

#ifndef ALKVALUE_H
#define ALKVALUE_H

// So we can save this object

#include <gmpxx.h>
#include <QString>
#include <kdemacros.h>

//#include <QDataStream.h>

#define ALKIMIA_EXPORT KDE_EXPORT

/**
  * This class represents a value within Alkimia
  *
  * @author Thomas Baumgart
  */
class ALKIMIA_EXPORT AlkValue
{
public:
  // TODO check if we still need those (they are a leftover from the old stuff)
  enum roundingMethod {
    RndNever = 0,
    RndFloor,
    RndCeil,
    RndTrunc,
    RndPromote,
    RndHalfDown,
    RndHalfUp,
    RndRound
  };

  // Con- / Destructors
  /**
    * This is the standard constructor of an AlkValue object.
    * The value will be initialized to 0.
    */
  AlkValue();

  /// The destructor
  ~AlkValue() {}

  /**
    * This constructor converts an int into an AlkValue. It can
    * also convert a rational number when a @a denom is supplied.
    *
    * @param num numerator of the rational number
    * @param denom denominator of the rational number (defaults to 1)
    */
  explicit AlkValue( const int num, const unsigned int denom = 1  );

  explicit AlkValue( const mpq_class& val );

  /**
    * This constructor converts a QString into an AlkValue.
    * Several formats are supported:
    *
    *  a) prices in the form "8 5/16"
    *  b) our own 'toString' format
    *  c) others

    * Others may be enclosed in "(" and ")" and treated as negative.
    * They may start or end with a dash and treated as negative.
    * The rightmost non-numeric character is treated as decimal symbol
    * All other non-numeric characters are skipped
    */
  AlkValue(const QString& str, const QChar& decimalSymbol);

  AlkValue(const double& dAmount);

  // assignment operators
  const AlkValue& operator=(const AlkValue& val);
  const AlkValue& operator=(int num);
  const AlkValue& operator=(double num);
  const AlkValue& operator=(const QString& str);

  // comparison
  bool operator==( const AlkValue& val ) const;
  bool operator!=( const AlkValue& val ) const;
  bool operator<( const AlkValue& val ) const;
  bool operator>( const AlkValue& val ) const;
  bool operator<=( const AlkValue& val ) const;
  bool operator>=( const AlkValue& val ) const;

  // calculation
  AlkValue operator+( const AlkValue& summand ) const;
  AlkValue operator-( const AlkValue& minuend ) const;
  AlkValue operator*( const AlkValue& factor ) const;
  AlkValue operator/( const AlkValue& divisor ) const;

  AlkValue operator*( int factor) const;

  // unary operators
  AlkValue operator-() const;
  AlkValue& operator+= ( const AlkValue&  val );
  AlkValue& operator-= ( const AlkValue&  val );
  AlkValue& operator/= ( const AlkValue&  val );
  AlkValue& operator*= ( const AlkValue&  val );

  // functions

  /// @return the absolute value of the AlkValue
  AlkValue abs(void) const;

  /// @return QString representation in form 'num/denom'.
  QString toString(void) const;

#if 0
  AlkValue convert(int denom = 100, const roundingMethod how = RndRound) const;
  double toDouble(void) const;

#endif


protected:
  mpq_class	m_val;

  ALKIMIA_EXPORT friend std::ostream &operator<<(std::ostream &, const AlkValue &);
  ALKIMIA_EXPORT friend QDataStream &operator<<(QDataStream &, const AlkValue &);
  ALKIMIA_EXPORT friend QDataStream &operator>>(QDataStream &, AlkValue &);

};




// std::ostream& operator<< (std::ostream&, const AlkValue&);

inline AlkValue::AlkValue() :
  m_val(0)
{
}

inline AlkValue::AlkValue(const int num, const unsigned int denom) :
  m_val(num, denom)
{
  m_val.canonicalize();
}

inline AlkValue::AlkValue(const mpq_class& val) :
  m_val(val)
{
  m_val.canonicalize();
}

inline AlkValue::AlkValue(const double& dAmount)
{
  m_val = dAmount;
  m_val.canonicalize();
}

inline AlkValue AlkValue::operator+(const AlkValue& right) const
{
  AlkValue result;
  mpq_add(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator-(const AlkValue& right) const
{
  AlkValue result;
  mpq_sub(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator*(const AlkValue& right) const
{
  AlkValue result;
  mpq_mul(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator/(const AlkValue& right) const
{
  AlkValue result;
  mpq_div(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator*(int factor) const
{
  AlkValue result;
  mpq_class right(factor);
  mpq_mul(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline const AlkValue& AlkValue::operator=(const AlkValue& right)
{
  m_val = right.m_val;
  return *this;
}

inline const AlkValue& AlkValue::operator=(int right)
{
  m_val = right;
  m_val.canonicalize();
  return *this;
}

inline const AlkValue& AlkValue::operator=(double right)
{
  m_val = right;
  m_val.canonicalize();
  return *this;
}

inline const AlkValue& AlkValue::operator=(const QString& right)
{
  m_val = AlkValue(right, '.').m_val;
  return *this;
}

inline AlkValue AlkValue::abs(void) const
{
  AlkValue result;
  mpq_abs(result.m_val.get_mpq_t(), m_val.get_mpq_t());
  return result;
}

inline  bool AlkValue::operator==( const AlkValue& val ) const
{
  return mpq_equal(m_val.get_mpq_t(), val.m_val.get_mpq_t());
}

inline  bool AlkValue::operator!=( const AlkValue& val ) const
{
  return !mpq_equal(m_val.get_mpq_t(), val.m_val.get_mpq_t());
}

inline  bool AlkValue::operator<( const AlkValue& val ) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) < 0 ? true : false;
}

inline  bool AlkValue::operator>( const AlkValue& val ) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) > 0 ? true : false;
}

inline  bool AlkValue::operator<=( const AlkValue& val ) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) <= 0 ? true : false;
}

inline  bool AlkValue::operator>=( const AlkValue& val ) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) >= 0 ? true : false;
}

inline AlkValue AlkValue::operator-() const
{
  AlkValue result;
  mpq_neg(result.m_val.get_mpq_t(), m_val.get_mpq_t());
  return result;
}

inline AlkValue& AlkValue::operator+=(const AlkValue& right)
{
  mpq_add(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

inline AlkValue& AlkValue::operator-=(const AlkValue& right)
{
  mpq_sub(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

inline AlkValue& AlkValue::operator*=(const AlkValue& right)
{
  mpq_mul(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

inline AlkValue& AlkValue::operator/=(const AlkValue& right)
{
  mpq_div(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

#endif

