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

#include "alkquoteitemtest.h"

#include "alkquoteitem.h"
#include "test.h"

QTEST_GUILESS_MAIN(AlkQuoteItemTest)

void AlkQuoteItemTest::init()
{
}

void AlkQuoteItemTest::cleanup()
{
}

void AlkQuoteItemTest::emptyCtor()
{
    AlkQuoteItem item;

    QVERIFY(item.symbol().isEmpty());
    QVERIFY(!item.dateTime().isValid());
    QCOMPARE(item.currentValue(), AlkValue(0, 1));
    QCOMPARE(item.openingValue(), AlkValue(0, 1));
    QCOMPARE(item.lowValue(), AlkValue(0, 1));
    QCOMPARE(item.highValue(), AlkValue(0, 1));
    QCOMPARE(item.closingValue(), AlkValue(0, 1));
    QCOMPARE(item.volume(), AlkValue(0, 1));
    QCOMPARE(item.marketCap(), AlkValue(0, 1));
    QCOMPARE(item.earningsPerShare(), AlkValue(0, 1));
    QCOMPARE(item.changeToday(), AlkValue(0, 1));
    QCOMPARE(item.ebitda(), AlkValue(0, 1));
    QVERIFY(item.recordId().isEmpty());
}

void AlkQuoteItemTest::copyCtor()
{
    AlkQuoteItem item;
    QString symbol = QString("TESTSYMBOL");
    QDateTime dateTime = QDateTime::currentDateTime();
    AlkValue currentValue(12, 1);
    AlkValue opening(1, 1);
    AlkValue lowValue(1, 2);
    AlkValue highValue(5, 1);
    AlkValue closing(4, 1);
    AlkValue volume(1000, 1);
    AlkValue marketCap(500, 1);
    AlkValue earnings(556, 1);
    AlkValue change(4, 1);
    AlkValue ebitda(67, 1);
    QString recordId("A1337");

    item.setSymbol(symbol);
    item.setDateTime(dateTime);
    item.setCurrentValue(currentValue);
    item.setOpeningValue(opening);
    item.setLowValue(lowValue);
    item.setHighValue(highValue);
    item.setClosingValue(closing);
    item.setVolume(volume);
    item.setMarketCap(marketCap);
    item.setEarningsPerShare(earnings);
    item.setChangeToday(change);
    item.setEbitda(ebitda);
    item.setRecordId(recordId);

    AlkQuoteItem itemCopy(item);

    QCOMPARE(itemCopy.symbol(), symbol);
    QCOMPARE(itemCopy.dateTime(), dateTime);
    QCOMPARE(itemCopy.currentValue(), currentValue);
    QCOMPARE(itemCopy.openingValue(), opening);
    QCOMPARE(itemCopy.lowValue(), lowValue);
    QCOMPARE(itemCopy.highValue(), highValue);
    QCOMPARE(itemCopy.closingValue(), closing);
    QCOMPARE(itemCopy.volume(), volume);
    QCOMPARE(itemCopy.marketCap(), marketCap);
    QCOMPARE(itemCopy.earningsPerShare(), earnings);
    QCOMPARE(itemCopy.changeToday(), change);
    QCOMPARE(itemCopy.ebitda(), ebitda);
    QCOMPARE(itemCopy.recordId(), recordId);
}

void AlkQuoteItemTest::settersAndGetters()
{
    AlkQuoteItem item;
    QString symbol("TESTSYMBOL");
    QDateTime dateTime = QDateTime::currentDateTime();
    AlkValue currentValue(12, 1);
    AlkValue opening(1, 1);
    AlkValue lowValue(1, 2);
    AlkValue highValue(5, 1);
    AlkValue closing(4, 1);
    AlkValue volume(1000, 1);
    AlkValue marketCap(500, 1);
    AlkValue earnings(556, 1);
    AlkValue change(4, 1);
    AlkValue ebitda(67, 1);
    QString recordId("A1337");

    item.setSymbol(symbol);
    item.setDateTime(dateTime);
    item.setCurrentValue(currentValue);
    item.setOpeningValue(opening);
    item.setLowValue(lowValue);
    item.setHighValue(highValue);
    item.setClosingValue(closing);
    item.setVolume(volume);
    item.setMarketCap(marketCap);
    item.setEarningsPerShare(earnings);
    item.setChangeToday(change);
    item.setEbitda(ebitda);
    item.setRecordId(recordId);

    QCOMPARE(item.symbol(), symbol);
    QCOMPARE(item.dateTime(), dateTime);
    QCOMPARE(item.currentValue(), currentValue);
    QCOMPARE(item.openingValue(), opening);
    QCOMPARE(item.lowValue(), lowValue);
    QCOMPARE(item.highValue(), highValue);
    QCOMPARE(item.closingValue(), closing);
    QCOMPARE(item.volume(), volume);
    QCOMPARE(item.marketCap(), marketCap);
    QCOMPARE(item.earningsPerShare(), earnings);
    QCOMPARE(item.changeToday(), change);
    QCOMPARE(item.ebitda(), ebitda);
    QCOMPARE(item.recordId(), recordId);
}

void AlkQuoteItemTest::qDbusArgument()
{
    AlkQuoteItem item;
    QString symbol("TESTSYMBOL");
    QDateTime dateTime = QDateTime::currentDateTime();
    AlkValue opening(1, 1);
    AlkValue lowValue(1, 2);
    AlkValue highValue(5, 1);
    AlkValue closing(4, 1);
    AlkValue volume(1000, 1);
    AlkValue marketCap(500, 1);

    item.setSymbol(symbol);
    item.setDateTime(dateTime);
    item.setOpeningValue(opening);
    item.setLowValue(lowValue);
    item.setHighValue(highValue);
    item.setClosingValue(closing);
    item.setVolume(volume);
    item.setMarketCap(marketCap);

    QDBusArgument arg;

    arg << item;

// FIXME: reading from endArg throws a write-only warning and the test fails
/*const QDBusArgument endArg(arg);

AlkQuoteItem itemEnd;
endArg >> itemEnd;

qDebug() << "symbol: " << itemEnd.symbol();

QVERIFY(symbol == itemEnd.symbol());
QVERIFY(dateTime == itemEnd.dateTime());
QVERIFY(opening == itemEnd.openingValue());
QVERIFY(lowValue == itemEnd.lowValue());
QVERIFY(highValue == itemEnd.highValue());
QVERIFY(closing == itemEnd.closingValue());
QVERIFY(volume == itemEnd.volume());
QVERIFY(marketCap == itemEnd.marketCap());*/
}
