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

// Workaround: include before gmpxx.h to fix build with gcc-4.9
/** @todo When gmp version is higer than 5.1.3, remove cstddef include */
#include <cstddef>
#include <gmpxx.h>                 // krazy:exclude=camelcase
#include <QString>
#include <QSharedDataPointer>

#include "alk_export.h"

/**
  * This class represents a financial value within Alkimia.
  * It can be used to represent balances, shares, amounts etc.
  *
  * @author Thomas Baumgart
  */
class ALK_EXPORT AlkValue
{
public:
  enum RoundingMethod {
    RoundNever = 0,              /**<
                                   * Don't do any rounding, simply truncate and
                                   * print a warning in case of a remainder.
                                   * Otherwise the same as RoundTrunc.
                                   */

    RoundFloor,                  /**<
                                   * Round to the largest integral value not
                                   * greater than @p this.
                                   * e.g. 0.5 -> 0.0 and -0.5 -> -1.0
                                   */

    RoundCeil,                   /**<
                                   * Round to the smallest integral value not
                                   * less than @p this.
                                   * e.g. 0.5 -> 1.0 and -0.5 -> -0.0
                                   */

    RoundTruncate,               /**<
                                   * No rounding, simply truncate any fraction
                                   */

    RoundPromote,                /**<
                                   * Use RoundCeil for positive and RoundFloor
                                   * for negative values of @p this.
                                   * e.g. 0.5 -> 1.0 and -0.5 -> -1.0
                                   */

    RoundHalfDown,               /**<
                                   * Round up or down with the following
                                   * constraints:
                                   * 0.1 .. 0.5 -> 0.0 and 0.6 .. 0.9 -> 1.0
                                   */

    RoundHalfUp,                 /**<
                                   * Round up or down with the following
                                   * constraints:
                                   * 0.1 .. 0.4 -> 0.0 and 0.5 .. 0.9 -> 1.0
                                   */

    RoundRound                     /**<
                                   * Use RoundHalfDown for 0.1 .. 0.4 and
                                   * RoundHalfUp for 0.6 .. 0.9. Use RoundHalfUp
                                   * for 0.5 in case the resulting numerator
                                   * is odd, RoundHalfDown in case the resulting
                                   * numerator is even.
                                   * e.g. 0.5 -> 0.0 and 1.5 -> 2.0
                                   */
  };

  // Constructors / Destructor
  /**
    * This is the standard constructor of an AlkValue object.
    * The value will be initialized to 0.
    */
  AlkValue();

  /// The destructor
  ~AlkValue();

  /// Copy constructor
  AlkValue(const AlkValue &val);

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
    * of 1.23. The rounding method is @p RoundRound.
    *
    * @sa AlkValue::convertDenominator()
    *
    * @param val the double value
    * @param denom the denominator of the resulting AlkValue
    *
    * @note In case one wants to use the number of decimal places
    * to specify the length of the fractional part, use
    *
    * @code
    *  AlkValue alk(1.234, AlkValue::precisionToDenominator(2).get_ui());
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
    * the @a how argument and defaults to @p RoundRound.
    */
  AlkValue convertDenominator(const int denom = 100, const RoundingMethod how = RoundRound) const;

  /**
    * This is a convenience function for convertDenom but instead of providing
    * the new denominator one provides the number of digits for the @a precision.
    * This value defaults to 2.  The rounding method used is controlled by
    * the @a how argument and defaults to @p RoundRound.
    */
  AlkValue convertPrecision(const int precision = 2, const RoundingMethod how = RoundRound) const;

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
  AlkValue abs() const;

  /// @return QString representation in form '[-]num/denom'.
  QString toString() const;

  /**
    * This method transforms the AlkValue into its canonicalized
    * form by reducing it to the smallest denominator. Example:
    * 25/100 will be converted to 1/4. Use this function at the
    * end of a longer calculation as all AlkValue methods require
    * the object to be in the canonicalized form. For speed purposes
    * the conversion is not performed before each operation.
    *
    * @return const reference to the object
    */
  const AlkValue& canonicalize();

  /// convert a denominator to a precision
  /// e.g. 100 -> 2, 1000 -> 3
  /// in case of a negative @a denom, the function returns 0
  static mpz_class denominatorToPrecision(mpz_class denom);

  /// convert a precision to the corresponding denominator
  /// e.g. 2 -> 100, 4 -> 10000
  /// in case of a negative @a prec, the function returns 1
  static mpz_class precisionToDenominator(mpz_class prec);

protected:
  /// \internal unit test class
  friend class AlkValueTest;

  /// provides an access method to the private value storage
  /// for derived classes
  const mpq_class &valueRef() const;
  mpq_class &valueRef();

private:
  /// \internal d-pointer class.
  class Private;
  /// \internal d-pointer instance.
  QSharedDataPointer<Private> d;
  /// \internal shared zero value.
  static QSharedDataPointer<AlkValue::Private>& sharedZero();

  // The following methods are not implemented (yet)
  // ALKIMIA_EXPORT friend QDataStream &operator<<(QDataStream &, const AlkValue &);
  // ALKIMIA_EXPORT friend QDataStream &operator>>(QDataStream &, AlkValue &);
};

#endif

