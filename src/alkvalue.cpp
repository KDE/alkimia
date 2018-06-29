/***************************************************************************
 *   Copyright 2010  Thomas Baumgart  tbaumgart@kde.org                    *
 *   Copyright 2018  Thomas Baumgart  tbaumgart@kde.org                    *
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
#include <QSharedData>

class AlkValue::Private : public QSharedData
{
public:
    Private()
    {
    }

    Private(const Private &other) : QSharedData(other)
        , m_val(other.m_val)
    {
    }

    mpq_class m_val;
};

/**
  * Helper function to convert an mpq_class object into
  * its internal QString representation. Mainly used for
  * debugging.
  */
static QString mpqToString(const mpq_class &val)
{
    char *p = 0;
    // use the gmp provided conversion routine
    gmp_asprintf(&p, "%Qd", val.get_mpq_t());

    // convert it into a QString
    QString result = QString::fromLatin1(p);

    // and free up the resources allocated by gmp_asprintf
    void (*freefunc)(void *, size_t);
    mp_get_memory_functions(NULL, NULL, &freefunc);
    (*freefunc)(p, std::strlen(p) + 1);

    if (!result.contains(QLatin1Char('/'))) {
        result += QString::fromLatin1("/1");
    }

    // done
    return result;
}

#if 0
/**
  * Helper function to convert an mpz_class object into
  * its internal QString representation. Mainly used for
  * debugging.
  */
static QString mpzToString(const mpz_class &val)
{
    char *p = 0;
    // use the gmp provided conversion routine
    gmp_asprintf(&p, "%Zd", val.get_mpz_t());

    // convert it into a QString
    QString result(QString::fromLatin1(p));

    // and free up the resources allocated by gmp_asprintf
    __gmp_freefunc_t freefunc;
    mp_get_memory_functions(NULL, NULL, &freefunc);
    (*freefunc)(p, std::strlen(p) + 1);

    // done
    return result;
}

#endif

QSharedDataPointer<AlkValue::Private> &AlkValue::sharedZero()
{
    static QSharedDataPointer<AlkValue::Private> sharedZeroPointer(new AlkValue::Private);
    return sharedZeroPointer;
}

AlkValue::AlkValue()
    : d(sharedZero())
{
}

AlkValue::AlkValue(const AlkValue &val)
    : d(val.d)
{
}

AlkValue::AlkValue(const int num, const unsigned int denom)
    : d(new Private)
{
    d->m_val = mpq_class(num, denom);
    d->m_val.canonicalize();
}

AlkValue::AlkValue(const mpz_class &num, const mpz_class &denom)
    : d(new Private)
{
    mpz_set(d->m_val.get_num_mpz_t(), num.get_mpz_t());
    mpz_set(d->m_val.get_den_mpz_t(), denom.get_mpz_t());
    d->m_val.canonicalize();
}

AlkValue::AlkValue(const mpq_class &val)
    : d(new Private)
{
    d->m_val = val;
    d->m_val.canonicalize();
}

AlkValue::AlkValue(const double &dAmount, const unsigned int denom)
    : d(new Private)
{
    d->m_val = dAmount;
    d->m_val.canonicalize();
    if (denom != 0) {
        *this = convertDenominator(denom);
    }
}

AlkValue::AlkValue(const QString &str, const QChar &decimalSymbol)
    : d(new Private)
{
    // empty strings are easy
    if (str.isEmpty()) {
        return;
    }

    // take care of mixed prices of the form "5 8/16" as well
    // as own internal string representation
    QRegExp regExp(QLatin1String("^((\\d+)\\s+|-)?(\\d+/\\d+)"));
    //                               +-#2-+        +---#3----+
    //                              +-----#1-----+
    if (regExp.indexIn(str) > -1) {
        d->m_val = qPrintable(str.mid(regExp.pos(3)));
        d->m_val.canonicalize();
        const QString &part1 = regExp.cap(1);
        if (!part1.isEmpty()) {
            if (part1 == QLatin1String("-")) {
                mpq_neg(d->m_val.get_mpq_t(), d->m_val.get_mpq_t());
            } else {
                mpq_class summand(qPrintable(part1));
                mpq_add(d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), summand.get_mpq_t());
                d->m_val.canonicalize();
            }
        }
        return;
    }

    // qDebug("we got '%s' to convert", qPrintable(str));
    // everything else gets down here
    const QString negChars = QString::fromLatin1("\\-\\(\\)");
    const QString validChars = QString::fromLatin1("\\d\\%1%2").arg(decimalSymbol, negChars);
    QRegExp invCharSet(QString::fromLatin1("[^%1]").arg(validChars));
    QRegExp negCharSet(QString::fromLatin1("[%1]").arg(negChars));

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

    // take care of any fractional part
    pos = res.indexOf(decimalSymbol);
    int len = res.length();
    QString fraction = QString::fromLatin1("/1");
    if ((pos != -1) && (pos < len)) {
        fraction += QString(len - pos - 1, QLatin1Char('0'));
        res.remove(pos, 1);
    }

    // check if the resulting numerator contains any leading zeros ...
    int cnt = 0;
    len = res.length() - 1;
    while (res[cnt] == QLatin1Char('0') && cnt < len) {
        ++cnt;
    }

    // ... and remove them
    if (cnt) {
        res.remove(0, cnt);
    }

    // in case the numerator is empty, we convert it to "0"
    if (res.isEmpty()) {
        res = QLatin1Char('0');
    }
    res += fraction;

    // looks like we now have a pretty normalized string that we
    // can convert right away
    // qDebug("and try to convert '%s'", qPrintable(res));
    try {
        d->m_val = mpq_class(qPrintable(res));
    } catch (const std::invalid_argument &) {
        qWarning("Invalid argument '%s' to mpq_class() in AlkValue. Arguments to ctor: '%s', '%c'", qPrintable(
                     res), qPrintable(str), decimalSymbol.toLatin1());
        d->m_val = mpq_class(0);
    }
    d->m_val.canonicalize();

    // now we make sure that we use the right sign
    if (isNegative) {
        d->m_val = -d->m_val;
    }
}

AlkValue::~AlkValue()
{
}

QString AlkValue::toString() const
{
    return mpqToString(d->m_val);
}

double AlkValue::toDouble() const
{
  return d->m_val.get_d();
}

AlkValue AlkValue::convertDenominator(int _denom, const RoundingMethod how) const
{
    AlkValue in(*this);
    mpz_class in_num(mpq_numref(in.d->m_val.get_mpq_t()));

    AlkValue out; // initialize to zero

    int sign = sgn(in_num);
    if (sign != 0) {
        // sign is either -1 for negative numbers or +1 in all other cases

        AlkValue temp;
        mpz_class denom = _denom;
        // only process in case the denominators are different
        if (mpz_cmpabs(denom.get_mpz_t(), mpq_denref(d->m_val.get_mpq_t())) != 0) {
            mpz_class in_denom(mpq_denref(in.d->m_val.get_mpq_t()));
            mpz_class out_num, out_denom;

            if (sgn(in_denom) == -1) { // my denom is negative
                in_num = in_num * (-in_denom);
                in_num = 1;
            }

            mpz_class remainder;
            int denom_neg = 0;

            // if the denominator is less than zero, we are to interpret it as
            // the reciprocal of its magnitude.
            if (sgn(denom) < 0) {
                mpz_class temp_a;
                mpz_class temp_bc;
                denom = -denom;
                denom_neg = 1;
                temp_a = ::abs(in_num);
                temp_bc = in_denom * denom;
                remainder = temp_a % temp_bc;
                out_num = temp_a / temp_bc;
                out_denom = denom;
            } else {
                temp = AlkValue(denom, in_denom);
                // the canonicalization required here is part of the ctor
                // temp.d->m_val.canonicalize();

                out_num = ::abs(in_num * temp.d->m_val.get_num());
                remainder = out_num % temp.d->m_val.get_den();
                out_num = out_num / temp.d->m_val.get_den();
                out_denom = denom;
            }

            if (remainder != 0) {
                switch (how) {
                case RoundFloor:
                    if (sign < 0) {
                        out_num = out_num + 1;
                    }
                    break;

                case RoundCeil:
                    if (sign > 0) {
                        out_num = out_num + 1;
                    }
                    break;

                case RoundTruncate:
                    break;

                case RoundPromote:
                    out_num = out_num + 1;
                    break;

                case RoundHalfDown:
                    if (denom_neg) {
                        if ((2 * remainder) > (in_denom * denom)) {
                            out_num = out_num + 1;
                        }
                    } else if ((2 * remainder) > (temp.d->m_val.get_den())) {
                        out_num = out_num + 1;
                    }
                    break;

                case RoundHalfUp:
                    if (denom_neg) {
                        if ((2 * remainder) >= (in_denom * denom)) {
                            out_num = out_num + 1;
                        }
                    } else if ((2 * remainder) >= temp.d->m_val.get_den()) {
                        out_num = out_num + 1;
                    }
                    break;

                case RoundRound:
                    if (denom_neg) {
                        if ((remainder * 2) > (in_denom * denom)) {
                            out_num = out_num + 1;
                        } else if ((2 * remainder) == (in_denom * denom)) {
                            if ((out_num % 2) != 0) {
                                out_num = out_num + 1;
                            }
                        }
                    } else {
                        if ((remainder * 2) > temp.d->m_val.get_den()) {
                            out_num = out_num + 1;
                        } else if ((2 * remainder) == temp.d->m_val.get_den()) {
                            if ((out_num % 2) != 0) {
                                out_num = out_num + 1;
                            }
                        }
                    }
                    break;

                case RoundNever:
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

AlkValue AlkValue::convertPrecision(int prec, const RoundingMethod how) const
{
    return convertDenominator(precisionToDenominator(prec).get_si(), how);
}

mpz_class AlkValue::denominatorToPrecision(mpz_class denom)
{
    mpz_class rc = 0;
    while (denom > 1) {
        ++rc;
        denom /= 10;
    }
    return rc;
}

mpz_class AlkValue::precisionToDenominator(mpz_class prec)
{
    mpz_class denom = 1;
    while ((prec--) > 0) {
        denom *= 10;
    }
    return denom;
}

const AlkValue &AlkValue::canonicalize()
{
    d->m_val.canonicalize();
    return *this;
}

AlkValue AlkValue::operator+(const AlkValue &right) const
{
    AlkValue result;
    mpq_add(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

AlkValue AlkValue::operator-(const AlkValue &right) const
{
    AlkValue result;
    mpq_sub(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

AlkValue AlkValue::operator*(const AlkValue &right) const
{
    AlkValue result;
    mpq_mul(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

AlkValue AlkValue::operator/(const AlkValue &right) const
{
    AlkValue result;
    mpq_div(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

AlkValue AlkValue::operator*(int factor) const
{
    AlkValue result;
    mpq_class right(factor);
    mpq_mul(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

const AlkValue &AlkValue::operator=(const AlkValue &right)
{
    d = right.d;
    return *this;
}

const AlkValue &AlkValue::operator=(int right)
{
    d->m_val = right;
    d->m_val.canonicalize();
    return *this;
}

const AlkValue &AlkValue::operator=(double right)
{
    d->m_val = right;
    d->m_val.canonicalize();
    return *this;
}

const AlkValue &AlkValue::operator=(const QString &right)
{
    AlkValue other(right, QLatin1Char('.'));
    d->m_val = other.d->m_val;
    return *this;
}

AlkValue AlkValue::abs() const
{
    AlkValue result;
    mpq_abs(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

bool AlkValue::operator==(const AlkValue &val) const
{
    if (d == val.d) {
        return true;
    }
    return mpq_equal(d->m_val.get_mpq_t(), val.d->m_val.get_mpq_t());
}

bool AlkValue::operator!=(const AlkValue &val) const
{
    if (d == val.d) {
        return false;
    }
    return !mpq_equal(d->m_val.get_mpq_t(), val.d->m_val.get_mpq_t());
}

bool AlkValue::operator<(const AlkValue &val) const
{
    return mpq_cmp(d->m_val.get_mpq_t(), val.d->m_val.get_mpq_t()) < 0 ? true : false;
}

bool AlkValue::operator>(const AlkValue &val) const
{
    return mpq_cmp(d->m_val.get_mpq_t(), val.d->m_val.get_mpq_t()) > 0 ? true : false;
}

bool AlkValue::operator<=(const AlkValue &val) const
{
    return mpq_cmp(d->m_val.get_mpq_t(), val.d->m_val.get_mpq_t()) <= 0 ? true : false;
}

bool AlkValue::operator>=(const AlkValue &val) const
{
    return mpq_cmp(d->m_val.get_mpq_t(), val.d->m_val.get_mpq_t()) >= 0 ? true : false;
}

AlkValue AlkValue::operator-() const
{
    AlkValue result;
    mpq_neg(result.d->m_val.get_mpq_t(), d->m_val.get_mpq_t());
    result.d->m_val.canonicalize();
    return result;
}

AlkValue &AlkValue::operator+=(const AlkValue &right)
{
    mpq_add(d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    d->m_val.canonicalize();
    return *this;
}

AlkValue &AlkValue::operator-=(const AlkValue &right)
{
    mpq_sub(d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    d->m_val.canonicalize();
    return *this;
}

AlkValue &AlkValue::operator*=(const AlkValue &right)
{
    mpq_mul(d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    d->m_val.canonicalize();
    return *this;
}

AlkValue &AlkValue::operator/=(const AlkValue &right)
{
    mpq_div(d->m_val.get_mpq_t(), d->m_val.get_mpq_t(), right.d->m_val.get_mpq_t());
    d->m_val.canonicalize();
    return *this;
}

const mpq_class &AlkValue::valueRef() const
{
    return d->m_val;
}

mpq_class &AlkValue::valueRef()
{
    return d->m_val;
}
