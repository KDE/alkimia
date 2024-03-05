/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2020 Thomas Baumgart <tbaumgart@kde.org>

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequoteprivatetest.h"

#include "alkonlinequote_p.h"
#include "alkonlinequotesprofile.h"

#include <QtTest/QtTest>

QTEST_MAIN(AlkOnlineQuotePrivateTest)

AlkOnlineQuotePrivateTest::AlkOnlineQuotePrivateTest()
    : AlkOnlineQuote(new AlkOnlineQuotesProfile("alkimia"))
{
}

void AlkOnlineQuotePrivateTest::testParsePrice()
{
    AlkOnlineQuote::Private &p = d_ptr();

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12345.675"));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice(" 12 345.675"));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345.675"));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345675"));
    QCOMPARE(p.m_price, 12.3456750);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345"));
    QCOMPARE(p.m_price, 12.3450);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345675", AlkOnlineQuoteSource::Period));
    QCOMPARE(p.m_price, 12345675.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345", AlkOnlineQuoteSource::Period));
    QCOMPARE(p.m_price, 12345.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12345"));
    QCOMPARE(p.m_price, 12345.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("109728364361224110982112411097810241109340924110963082411093707241109550624110955052411094404241109230324110949022411102301241110383123111056302311105629231110692823111118272311103026231110302523111005242311102323231110262223111003212311094920231109701923110927182311091017231109091623110909152311099"));
    QCOMPARE(p.m_price, 1.0972836436122411e+299);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("1.0971 &#8364; = &#36; Print Exchange Rate Chart Euro to Dollar - EUR/USD Invert DateExchange Rate12 Jan 241 EUR = 1.0982 USD11 Jan 241 EUR = 1.0978 USD10 Jan 241 EUR = 1.0934 USD09 Jan 241 EUR = 1.0963 USD08 Jan 241 EUR = 1.0937 USD07 Jan 241 EUR = 1.0955 USD06 Jan 241 EUR = 1.0955 USD05 Jan 241 EU"));
    QCOMPARE(p.m_price, 1.0971);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(!p.parsePrice("Print Exchange Rate Chart Euro to Dollar"));
    QCOMPARE(p.m_price, 0.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Price);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    // workaround for msvc 2048 character string limit
    QFile f(":/alkonlinequoteprivatetest.data");
    QVERIFY(f.open(QIODevice::ReadOnly));
    QVERIFY(!p.parsePrice(f.readAll()));
    QCOMPARE(p.m_price, 0.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Price);

    // comma as decimal separator
    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345.675", AlkOnlineQuoteSource::Comma));
    QCOMPARE(p.m_price, 12345.675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12,345675", AlkOnlineQuoteSource::Comma));
    QCOMPARE(p.m_price, 12.345675);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("12345675", AlkOnlineQuoteSource::Comma));
    QCOMPARE(p.m_price, 12345675.0);
    QVERIFY(errors() & AlkOnlineQuote::Errors::Success);
}

class SingleQuoteReceiver : public QObject
{
    Q_OBJECT
public:
    QDate _date;
    double _price;
    SingleQuoteReceiver(QDate date, double price)
        : _date(date)
        , _price(price)
    {}

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void quote(QString id, QString symbol, QDate date, double price)
    {
        qDebug() << "comparing quote" << id << symbol;
        QCOMPARE(date, _date);
        QCOMPARE(price, _price);
        Q_EMIT finished();
    }
};

void AlkOnlineQuotePrivateTest::testParseQuoteCSVSingleLine()
{
    AlkOnlineQuote::Private &p = d_ptr();

    AlkOnlineQuoteSource source("test", "", "", AlkOnlineQuoteSource::Symbol, "#2", "#1", "%d-%m-%y", AlkOnlineQuoteSource::CSV);
    p.m_source = source;
    QString quotedata =
        "22-01-24,1.0906\n"
        ;
    SingleQuoteReceiver receiver(QDate::fromString("22-01-2024", "dd-MM-yyyy"), 1.0906);
    connect(this, SIGNAL(quote(QString,QString,QDate,double)), &receiver, SLOT(quote(QString,QString,QDate,double)));
    QVERIFY(p.parseQuoteCSV(quotedata));
}

class MultipleQuotesReceiver : public QObject
{
    Q_OBJECT
public:
    const AlkDatePriceMap &_prices;
    MultipleQuotesReceiver(const AlkDatePriceMap &prices)
        : _prices(prices)
    {}

Q_SIGNALS:
    void finished();

public Q_SLOTS:
    void quotes(const QString& id, const QString& symbol, const AlkDatePriceMap& prices)
    {
        qDebug() << "comparing" << prices.size() << "quotes" << id << symbol;
        QCOMPARE(prices.size(), _prices.size());
        QCOMPARE(prices.keys(), _prices.keys());
        for (auto &date: prices.keys()) {
            QCOMPARE(prices[date].toDouble(), _prices[date].toDouble());
        }
        Q_EMIT finished();
    }
};

void AlkOnlineQuotePrivateTest::testParseQuoteCSVMultipleLines()
{
    AlkOnlineQuote::Private &p = d_ptr();

    QString quotedata =
        "22-01-24,1.0906\n"
        "21-01-24,1.1906\n"
        ;
    AlkDatePriceMap map;
    map[QDate::fromString("22-01-2024", "dd-MM-yyyy")] = 1.0906;
    map[QDate::fromString("21-01-2024", "dd-MM-yyyy")] = 1.1906;
    MultipleQuotesReceiver receiver(map);
    connect(this, SIGNAL(quotes(QString,QString,AlkDatePriceMap)), &receiver, SLOT(quotes(QString,QString,AlkDatePriceMap)));
    QVERIFY(p.parseQuoteCSV(quotedata));
}

void AlkOnlineQuotePrivateTest::testParseQuoteCSVDateRange()
{
    AlkOnlineQuote::Private &p = d_ptr();

    QString quotedata =
        "22-01-24,1.0906\n"
        "21-01-24,1.1906\n"
        "20-01-24,1.4906\n"
        ;
    p.m_startDate = QDate::fromString("21-01-2024", "dd-MM-yyyy");
    p.m_endDate = QDate::fromString("21-01-2024", "dd-MM-yyyy");

    AlkDatePriceMap map;
    map[QDate::fromString("21-01-2024", "dd-MM-yyyy")] = 1.1906;
    MultipleQuotesReceiver multiReceiver(map);
    connect(this, SIGNAL(quotes(QString,QString,AlkDatePriceMap)), &multiReceiver, SLOT(quotes(QString,QString,AlkDatePriceMap)));

    SingleQuoteReceiver singleReceiver(p.m_startDate, 1.1906);
    connect(this, SIGNAL(quote(QString,QString,QDate,double)), &singleReceiver, SLOT(quote(QString,QString,QDate,double)));

    QVERIFY(p.parseQuoteCSV(quotedata));
}

#include "alkonlinequoteprivatetest.moc"
