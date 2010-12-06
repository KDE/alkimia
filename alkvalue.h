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

#include <gmpxx.h>                 // krazy:exclude=camelcase
#include <QString>
#include <kdemacros.h>

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
  enum RoundingMethod {
    RndNever = 0,                /**<
                                   * don't do any rounding, simply truncate and
                                   * print a warning in case of a remainder. Otherwise
                                   * the same as RndTrunc.
                                   */

    RndFloor,                    /**<
                                   * Round to the largest integral value not greater
                                   * than @p this.
                                   * e.g. 0.5 -> 0.0 and -0.5 -> -1.0
                                   */

    RndCeil,                     /**<
                                   * Round to the smallest integral value not less
                                   * than @p this.
                                   * e.g. 0.5 -> 1.0 and -0.5 -> -0.0
                                   */

    RndTrunc,                    /**<
                                   * No rounding, simply truncate any fraction
                                   */

    RndPromote,                  /**<
                                   * Use RndCeil for positive and RndFloor for
                                   * negative values of @p this.
                                   * e.g. 0.5 -> 1.0 and -0.5 -> -1.0
                                   */

    RndHalfDown,                 /**<
                                   * Round up or down with the following constraints:
                                   * 0.1 .. 0.5 -> 0.0 and 0.6 .. 0.9 -> 1.0
                                   */

    RndHalfUp,                   /**<
                                   * Round up or down with the following constraints:
                                   * 0.1 .. 0.4 -> 0.0 and 0.5 .. 0.9 -> 1.0
                                   */

    RndRound                     /**<
                                   * Use RndHalfDown for 0.1 .. 0.4 and RndHalfUp
                                   * for 0.6 .. 0.9. Use RndHalfUp for 0.5 in case
                                   * the resulting numerator is odd, RndHalfDown
                                   * in case the resulting numerator is even.
                                   * e.g. 0.5 -> 0.0 and 1.5 -> 2.0
                                   */
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
  explicit AlkValue(const int num, const unsigned int denom = 1);

  /**
    * Convenience ctor for usage with mpz_class objects as numerator
    * and denominator.
    *
    * @param num numerator of the rational number
    * @param denom denominator of the rational number (defaults to 1)
    */
  explicit AlkValue(const mpz_class &num, const mpz_class &denom);

  /**
    * Convenience ctor to create an AlkValue object based on an mpq_class object
    */
  explicit AlkValue(const mpq_class &val);

  /**
    * This constructor converts a double into an AlkValue. In case
    * a @a denom is supplied with a value different from zero, the
    * @a val will be rounded to be based on the supplied @a denom.
    * e.g. val = 1.234 and denom = 100 will construct an AlkValue
    * of 1.23. The rounding method is @p RndRound.
    *
    * @sa AlkValue::convertDenom()
    *
    * @param val the double value
    * @param denom the denominator of the resulting AlkValue
    *
    * @note In case one wants to use the number of precision digits
    * to specify the fractional length, use
    *
    * @code
    *  AlkValue alk(1.234, AlkValue::precToDenom(2).get_ui());
    *  // alk == 1.23
    * @endcode
    */
  explicit AlkValue(const double &val, const unsigned int denom = 0);

  /**
    * This constructor converts a QString into an AlkValue.
    * Several formats are supported:
    *
    * -# prices in the form "8 5/16"
    * -# our own toString() format
    * -# others

    * Others may be enclosed in "(" and ")" and treated as negative.
    * They may start or end with a dash and treated as negative.
    * The decimal symbols is identified as provided in @a decimalSymbol.
    * All other non-numeric characters are skipped
    */
  AlkValue(const QString &str, const QChar &decimalSymbol);

  /**
    * Returns the current value converted to the given @a denom (default is 100
    * or two digits of precision). The rounding method used is controlled by
    * the @a how argument and defaults to @p RndRound.
    */
  AlkValue convertDenom(const int denom = 100, const RoundingMethod how = RndRound) const;

  /**
    * This is a convenience function for convertDenom but instead of providing
    * the new denomiator one provides the number of digits for the @a precision.
    * This value defaults to 2.  The rounding method used is controlled by
    * the @a how argument and defaults to @p RndRound.
    */
  AlkValue convertPrec(const int precision = 2, const RoundingMethod how = RndRound) const;

  // assignment operators
  const AlkValue & operator=(const AlkValue &val);
  const AlkValue & operator=(int num);
  const AlkValue & operator=(double num);
  const AlkValue & operator=(const QString &str);

  // comparison
  bool operator==(const AlkValue &val) const;
  bool operator!=(const AlkValue &val) const;
  bool operator<(const AlkValue &val) const;
  bool operator>(const AlkValue &val) const;
  bool operator<=(const AlkValue &val) const;
  bool operator>=(const AlkValue &val) const;

  // calculation
  AlkValue operator+(const AlkValue &summand) const;
  AlkValue operator-(const AlkValue &minuend) const;
  AlkValue operator*(const AlkValue &factor) const;
  AlkValue operator/(const AlkValue &divisor) const;

  AlkValue operator*(int factor) const;

  // unary operators
  AlkValue operator-() const;
  AlkValue & operator+= (const AlkValue &val);
  AlkValue & operator-= (const AlkValue &val);
  AlkValue & operator/= (const AlkValue &val);
  AlkValue & operator*= (const AlkValue &val);

  // functions

  /// @return the absolute value of the AlkValue
  AlkValue abs(void) const;

  /// @return QString representation in form '[-]num/denom'.
  QString toString(void) const;

  /// convert a denomination to a precision
  /// e.g. 100 -> 2, 1000 -> 3
  /// in case of a negative @a denom, the function returns 0
  static mpz_class denomToPrec(mpz_class denom);

  /// convert a precision to the corresponding denominator
  /// e.g. 2 -> 100, 4 -> 10000
  /// in case of a negative @a prec, the function returns 1
  static mpz_class precToDenom(mpz_class prec);

protected:
  mpq_class m_val;

  // The following methods are not implemented (yet)
  // ALKIMIA_EXPORT friend QDataStream &operator<<(QDataStream &, const AlkValue &);
  // ALKIMIA_EXPORT friend QDataStream &operator>>(QDataStream &, AlkValue &);
};

inline AlkValue::AlkValue() :
    m_val(0)
{
}

inline AlkValue::AlkValue(const mpz_class &num, const mpz_class &denom) :
    m_val(num, denom)
{
  m_val.canonicalize();
}

inline AlkValue::AlkValue(const int num, const unsigned int denom) :
    m_val(num, denom)
{
  m_val.canonicalize();
}

inline AlkValue::AlkValue(const mpq_class &val) :
    m_val(val)
{
  m_val.canonicalize();
}

inline AlkValue::AlkValue(const double &dAmount, const unsigned int denom)
{
  m_val = dAmount;
  m_val.canonicalize();
  if (denom != 0) {
    *this = convertDenom(denom);
  }
}

inline AlkValue AlkValue::operator+(const AlkValue &right) const
{
  AlkValue result;
  mpq_add(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator-(const AlkValue &right) const
{
  AlkValue result;
  mpq_sub(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator*(const AlkValue &right) const
{
  AlkValue result;
  mpq_mul(result.m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  result.m_val.canonicalize();
  return result;
}

inline AlkValue AlkValue::operator/(const AlkValue &right) const
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

inline const AlkValue & AlkValue::operator=(const AlkValue & right)
{
  m_val = right.m_val;
  return *this;
}

inline const AlkValue & AlkValue::operator=(int right)
{
  m_val = right;
  m_val.canonicalize();
  return *this;
}

inline const AlkValue & AlkValue::operator=(double right)
{
  m_val = right;
  m_val.canonicalize();
  return *this;
}

inline const AlkValue & AlkValue::operator=(const QString & right)
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

inline  bool AlkValue::operator==(const AlkValue &val) const
{
  return mpq_equal(m_val.get_mpq_t(), val.m_val.get_mpq_t());
}

inline  bool AlkValue::operator!=(const AlkValue &val) const
{
  return !mpq_equal(m_val.get_mpq_t(), val.m_val.get_mpq_t());
}

inline  bool AlkValue::operator<(const AlkValue &val) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) < 0 ? true : false;
}

inline  bool AlkValue::operator>(const AlkValue &val) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) > 0 ? true : false;
}

inline  bool AlkValue::operator<=(const AlkValue &val) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) <= 0 ? true : false;
}

inline  bool AlkValue::operator>=(const AlkValue &val) const
{
  return mpq_cmp(m_val.get_mpq_t(), val.m_val.get_mpq_t()) >= 0 ? true : false;
}

inline AlkValue AlkValue::operator-() const
{
  AlkValue result;
  mpq_neg(result.m_val.get_mpq_t(), m_val.get_mpq_t());
  return result;
}

inline AlkValue & AlkValue::operator+=(const AlkValue & right)
{
  mpq_add(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

inline AlkValue & AlkValue::operator-=(const AlkValue & right)
{
  mpq_sub(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

inline AlkValue & AlkValue::operator*=(const AlkValue & right)
{
  mpq_mul(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

inline AlkValue & AlkValue::operator/=(const AlkValue & right)
{
  mpq_div(m_val.get_mpq_t(), m_val.get_mpq_t(), right.m_val.get_mpq_t());
  m_val.canonicalize();
  return *this;
}

#endif

