/***************************************************************************
 *   Copyright 2011  Alvaro Soliverez  asoliverez@kde.org                  *
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

#include "alkcompanytest.h"
#include "alkcompany.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkCompanyTest)

void AlkCompanyTest::init()
{
}

void AlkCompanyTest::cleanup()
{
}

void AlkCompanyTest::emptyCtor()
{
  AlkCompany company;

  QVERIFY(company.symbol().isEmpty());
  QVERIFY(company.name().isEmpty());
  QVERIFY(company.exchange().isEmpty());
  QVERIFY(company.type().isEmpty());
  QVERIFY(company.recordId().isEmpty());
}

void AlkCompanyTest::copyCtor()
{
  AlkCompany company;
  QString symbol = QString("TESTSYMBOL");
  QString name = QString("TESTNAME");
  QString exchange = QString("TESTEXCHANGE");
  QString type = QString("TESTTYPE");
  QString recordId = QString("A1337");

  company.setSymbol(symbol);
  company.setName(name);
  company.setExchange(exchange);
  company.setType(type);
  company.setRecordId(recordId);

  AlkCompany copy(company);

  QVERIFY(symbol == copy.symbol());
  QVERIFY(name == copy.name());
  QVERIFY(exchange == copy.exchange());
  QVERIFY(type == copy.type());
  QVERIFY(recordId == copy.recordId());
}

void AlkCompanyTest::settersAndGetters()
{
  AlkCompany company;
  QString symbol = QString("TESTSYMBOL");
  QString name = QString("TESTNAME");
  QString exchange = QString("TESTEXCHANGE");
  QString type = QString("TESTTYPE");
  QString recordId = QString("A1337");

  company.setSymbol(symbol);
  company.setName(name);
  company.setExchange(exchange);
  company.setType(type);
  company.setRecordId(recordId);

  QVERIFY(symbol == company.symbol());
  QVERIFY(name == company.name());
  QVERIFY(exchange == company.exchange());
  QVERIFY(type == company.type());
  QVERIFY(recordId == company.recordId());
}

void AlkCompanyTest::qDbusArgument()
{
// FIXME: reading from endArg throws a write-only warning and the test fails

}

