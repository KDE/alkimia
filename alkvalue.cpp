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

#include "alkvalue.h"

#include <iostream>
#include <QRegExp>

/**
  * Helper function to convert an mpq_class object into
  * its internal QString representation. Mainly used for
  * debugging.
  */
static QString mpqToString(const mpq_class & val)
{
  char *p = 0;
  // use the gmp provided conversion routine
  gmp_asprintf(&p, "%Qd", val.get_mpq_t());

  // convert it into a QString
  QString result(p);

  // and free up the resources allocated by gmp_asprintf
  __gmp_freefunc_t freefunc;
  mp_get_memory_functions(NULL, NULL, &freefunc);
  (*freefunc)(p, std::strlen(p) + 1);

  if (!result.contains('/')) {
    result += "/1";
  }

  // done
  return result;
}

/**
  * Helper function to convert an mpz_class object into
  * its internal QString representation. Mainly used for
  * debugging.
  */
static QString mpzToString(const mpz_class & val)
{
  char *p = 0;
  // use the gmp provided conversion routine
  gmp_asprintf(&p, "%Zd", val.get_mpz_t());

  // convert it into a QString
  QString result(p);

  // and free up the resources allocated by gmp_asprintf
  __gmp_freefunc_t freefunc;
  mp_get_memory_functions(NULL, NULL, &freefunc);
  (*freefunc)(p, std::strlen(p) + 1);

  // done
  return result;
}

AlkValue::AlkValue(const QString & str, const QChar & decimalSymbol) :
    m_val(0)
{
  // empty strings are easy
  if (str.isEmpty()) {
    return;
  }

  // take care of mixed prices of the form "5 8/16" as well
  // as own internal string representation
  QRegExp regExp("^((\\d+)\\s+|-)?(\\d+/\\d+)");
  //                +-#2-+        +---#3----+
  //               +-----#1-----+
  if (regExp.indexIn(str) > -1) {
    m_val = qPrintable(str.mid(regExp.pos(3)));
    m_val.canonicalize();
    const QString &part1 = regExp.cap(1);
    if (!part1.isEmpty()) {
      if (part1 == QLatin1String("-")) {
        mpq_neg(m_val.get_mpq_t(), m_val.get_mpq_t());

      } else {
        mpq_class summand(qPrintable(part1));
        mpq_add(m_val.get_mpq_t(), m_val.get_mpq_t(), summand.get_mpq_t());
        m_val.canonicalize();
      }
    }
    return;
  }

  // qDebug("we got '%s' to convert", qPrintable(str));
  // everything else gets down here
  const QString negChars = QString("\\-\\(\\)");
  const QString validChars = QString("\\d\\%1%2").arg(decimalSymbol, negChars);
  QRegExp invCharSet(QString("[^%1]").arg(validChars));
  QRegExp negCharSet(QString("[%1]").arg(negChars));

  QString res(str);
  // get rid of any character that is not allowed.
  res.remove(invCharSet);

  // qDebug("we reduced it to '%s'", qPrintable(res));
  // check if number is negative
  bool isNegative = false;
  if (res.indexOf(negCharSet) != -1) {
    isNegative = true;
    res.remove(negCharSet);
  }

  // qDebug("and modified it to '%s'", qPrintable(res));
  // if someone uses the decimal symbol more than once, we get
  // rid of them except the right most one
  int pos;
  while (res.count(decimalSymbol) > 1) {
    pos = res.indexOf(decimalSymbol);
    res.remove(pos, 1);
  }

  // take care of any fraction
  pos = res.indexOf(decimalSymbol);
  int len = res.length();
  QString fraction("/1");
  if ((pos != -1) && (pos < len)) {
    fraction += QString(len - pos - 1, '0');
    res.remove(pos, 1);

    // check if the resulting numerator contains any leading zeros ...
    int cnt = 0;
    while (res[cnt] == '0' && cnt < len - 2) {
      ++cnt;
    }

    // ... and remove them
    if (cnt) {
      res.remove(0, cnt);
    }
  }

  // in case the numerator is empty, we convert it to "0"
  if (res.isEmpty()) {
    res = '0';
  }
  res += fraction;

  // looks like we now have a pretty normalized string that we
  // can convert right away
  // qDebug("and try to convert '%s'", qPrintable(res));
  m_val = mpq_class(qPrintable(res));
  m_val.canonicalize();

  // now we make sure that we use the right sign
  if (isNegative) {
    m_val = -m_val;
  }
}

QString AlkValue::toString(void) const
{
  return mpqToString(m_val);
}

AlkValue AlkValue::convertDenom(int _denom, const RoundingMethod how) const
{
  AlkValue in(*this);
  mpz_class in_num(mpq_numref(in.m_val.get_mpq_t()));

  AlkValue out; // initialize to zero

  int sign = sgn(in_num);
  if (sign != 0) {
    // sign is either -1 for negative numbers or +1 in all other cases

    AlkValue temp;
    mpz_class denom = _denom;
    // only process in case the denominators are different
    if (mpz_cmpabs(denom.get_mpz_t(), mpq_denref(m_val.get_mpq_t())) != 0) {
      mpz_class in_denom(mpq_denref(in.m_val.get_mpq_t()));
      mpz_class out_num, out_denom;

      if (sgn(in_denom) == -1) { // my denom is negative
        in_num = in_num * (- in_denom);
        in_num = 1;
      }

      mpz_class remainder;
      int denom_neg = 0;

      // if the denominator is less than zero, we are to interpret it as
      // the reciprocal of its magnitude.
      if (sgn(denom) < 0) {
        mpz_class temp_a;
        mpz_class temp_bc;
        denom        = -denom;
        denom_neg    = 1;
        temp_a       = ::abs(in_num);
        temp_bc      = in_denom * denom;
        remainder    = temp_a % temp_bc;
        out_num      = temp_a / temp_bc;
        out_denom    = denom;
      } else {
        temp = AlkValue(denom, in_denom);
        // the canonicalization required here is part of the ctor
        // temp.m_val.canonicalize();

        out_num      = ::abs(in_num * temp.m_val.get_num());
        remainder    = out_num % temp.m_val.get_den();
        out_num      = out_num / temp.m_val.get_den();
        out_denom    = denom;
      }

      if (remainder != 0) {
        switch (how) {
          case RndFloor:
            if (sign < 0) {
              out_num = out_num + 1;
            }
            break;

          case RndCeil:
            if (sign > 0) {
              out_num = out_num + 1;
            }
            break;

          case RndTrunc:
            break;

          case RndPromote:
            out_num = out_num + 1;
            break;

          case RndHalfDown:
            if (denom_neg) {
              if ((2 * remainder) > (in_denom * denom)) {
                out_num = out_num + 1;
              }
            } else if ((2 * remainder) > (temp.m_val.get_den())) {
              out_num = out_num + 1;
            }
            break;

          case RndHalfUp:
            if (denom_neg) {
              if ((2 * remainder) >= (in_denom * denom)) {
                out_num = out_num + 1;
              }
            } else if ((2 * remainder) >= temp.m_val.get_den()) {
              out_num = out_num + 1;
            }
            break;

          case RndRound:
            if (denom_neg) {
              if ((remainder * 2) > (in_denom * denom)) {
                out_num = out_num + 1;
              } else if ((2 * remainder) == (in_denom * denom)) {
                if ((out_num % 2) != 0) {
                  out_num = out_num + 1;
                }
              }
            } else {
              if ((remainder * 2) > temp.m_val.get_den()) {
                out_num = out_num + 1;
              } else if ((2 * remainder) == temp.m_val.get_den()) {
                if ((out_num % 2) != 0) {
                  out_num = out_num + 1;
                }
              }
            }
            break;

          case RndNever:
            qWarning("AlkValue: have remainder \"%s\"->convert(%d, %d)",
                     qPrintable(toString()), _denom, how);
            break;
        }
      }

      // construct the new output value
      out = AlkValue(out_num * sign, out_denom);

    } else {
      out = *this;
    }
  }
  return out;
}

AlkValue AlkValue::convertPrec(int prec, const RoundingMethod how) const
{
  return convertDenom(precToDenom(prec).get_si(), how);
}

mpz_class AlkValue::denomToPrec(mpz_class denom)
{
  mpz_class rc = 0;
  while (denom > 1) {
    ++rc;
    denom /= 10;
  }
  return rc;
}

mpz_class AlkValue::precToDenom(mpz_class prec)
{
  mpz_class denom = 1;
  while ((prec--) > 0) {
    denom *= 10;
  }
  return denom;
}

