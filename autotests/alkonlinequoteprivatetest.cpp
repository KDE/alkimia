/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2020 Thomas Baumgart <tbaumgart@kde.org>

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequoteprivatetest.h"

#include "alkdebug.h"
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

    p.m_errors = AlkOnlineQuote::Errors::Success;
    QVERIFY(p.parsePrice("1.8247e-5", AlkOnlineQuoteSource::Period));
    QCOMPARE(p.m_price, 1.8247e-5);
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
        alkDebug() << "comparing quote"
                   << "with id" << id << "and symbol" << symbol;
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
        alkDebug() << "comparing" << prices.size() << "quotes"
                   << "with id" << id << "and symbol" << symbol;
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

void AlkOnlineQuotePrivateTest::testDateRangeInUrls()
{
    AlkOnlineQuote::Private &p = d_ptr();

    p.m_startDate = QDate::fromString("21-01-2024", "dd-MM-yyyy");
    p.m_endDate = QDate::fromString("23-01-2024", "dd-MM-yyyy");

    QDateTime startDate = QDateTime(p.m_startDate, QTime(), Qt::LocalTime);
    QDateTime endDate = QDateTime(p.m_endDate, QTime(23,59, 59, 999), Qt::LocalTime);
    qint64 startTimeUnix = startDate.toMSecsSinceEpoch() / 1000;
    qint64 endTimeUnix = endDate.toMSecsSinceEpoch() / 1000;

    QUrl url("http://unknown.host/path?start=%unix&end=%unix");
    QVERIFY(p.applyDateRange(url));
    QString urlStr = url.toEncoded();

    QVERIFY(urlStr.contains(QString(QLatin1String("start=%1")).arg(startTimeUnix)));
    QVERIFY(urlStr.contains(QString(QLatin1String("end=%1")).arg(endTimeUnix)));

    url.setUrl("http://unknown.host/path?start=%unix&end=%unix&some=%unix");
    QVERIFY(!p.applyDateRange(url));

    // only start date is defined
    url.setUrl("http://unknown.host/path?start=%unix");
    QVERIFY(p.applyDateRange(url));

    url.setUrl("http://unknown.host/path?start=%u&end=%u");
    QVERIFY(p.applyDateRange(url));
    urlStr = url.toEncoded();

    QVERIFY(urlStr.contains(QString(QLatin1String("start=%1")).arg(startTimeUnix)));
    QVERIFY(urlStr.contains(QString(QLatin1String("end=%1")).arg(endTimeUnix)));

    // utc time zone
    QDateTime startDateUTC = QDateTime(p.m_startDate, QTime(), Qt::UTC);
    QDateTime endDateUTC = QDateTime(p.m_endDate, QTime(23,59, 59, 999), Qt::UTC);
    qint64 startTimeUTC = startDateUTC.toMSecsSinceEpoch() / 1000;
    qint64 endTimeUTC = endDateUTC.toMSecsSinceEpoch() / 1000;

    url.setUrl("http://unknown.host/path?start=%utc&end=%utc");
    QVERIFY(p.applyDateRange(url));
    urlStr = url.toEncoded();

    QVERIFY(urlStr.contains(QString(QLatin1String("start=%1")).arg(startTimeUTC)));
    QVERIFY(urlStr.contains(QString(QLatin1String("end=%1")).arg(endTimeUTC)));

    // end date limit
    p.m_endDate = QDate::currentDate();
    endDate = QDateTime(p.m_endDate, QTime(23,59, 59, 999), Qt::LocalTime);
    endTimeUnix = endDate.toMSecsSinceEpoch() / 1000;

    url.setUrl("http://unknown.host/path?start=%u&end=%u");
    QVERIFY(p.applyDateRange(url));
    urlStr = url.toEncoded();

    QVERIFY(!urlStr.contains(QString(QLatin1String("end=%1")).arg(endTimeUnix)));
}

void AlkOnlineQuotePrivateTest::testGetSubTree()
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QSKIP("not implemented yet", SkipSingle);
#else
    QFile f(":/alkonlinequoteprivatetest-2.json");
    QVERIFY(f.open(QIODevice::ReadOnly));
    QString quotedata = f.readAll();

    auto jsonDoc = QJsonDocument::fromJson(quotedata.toLocal8Bit());

    QVERIFY(!jsonDoc.isNull());
    QVERIFY(jsonDoc.isObject());

    QVariantMap treeData = jsonDoc.object().toVariantMap();

    QVariant period;
    QString errorKey;

    QString path = "chart:unknown";
    bool result = AlkOnlineQuote::Private::getSubTree(treeData, path, period, errorKey);
    QCOMPARE(result, false);

    path = "chart:result:0:meta:currentTradingPeriod:regular";
    result = AlkOnlineQuote::Private::getSubTree(treeData, path, period, errorKey);
    QCOMPARE(result, true);
    QVariantMap map = period.value<QVariantMap>();
    QCOMPARE(map["timezone"].toString(), QString("EST"));
    QCOMPARE(map["start"].toInt(), 1740148200);
    QCOMPARE(map["end"].toInt(), 1740171600);

    // compatibility support
    path = "chart:result:meta:currentTradingPeriod:regular";
    result = AlkOnlineQuote::Private::getSubTree(treeData, path, period, errorKey);
    QCOMPARE(result, true);
    map = period.value<QVariantMap>();
    QCOMPARE(map["timezone"].toString(), QString("EST"));
    QCOMPARE(map["start"].toInt(), 1740148200);
    QCOMPARE(map["end"].toInt(), 1740171600);
#endif
}

void AlkOnlineQuotePrivateTest::testParseQuoteJson()
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QSKIP("not implemented yet", SkipSingle);
#else
    AlkOnlineQuote::Private &p = d_ptr();

    AlkOnlineQuoteSource source("test", "", "", AlkOnlineQuoteSource::Symbol, "chart:result:indicators:quote:open", "chart:result:timestamp", "%u", AlkOnlineQuoteSource::JSON);
    p.m_source = source;
    p.m_startDate = QDate::fromString("21-10-2024", "dd-MM-yyyy");
    p.m_endDate = QDate::fromString("25-10-2024", "dd-MM-yyyy");

    QFile f(":/alkonlinequoteprivatetest.json");
    QVERIFY(f.open(QIODevice::ReadOnly));
    QString quotedata = f.readAll();

    AlkDatePriceMap map;
    map[QDate::fromString("23-10-2024", "dd-MM-yyyy")] = 61807.7109375;
    map[QDate::fromString("24-10-2024", "dd-MM-yyyy")] = 61798.4375;
    MultipleQuotesReceiver multiReceiver(map);
    connect(this, SIGNAL(quotes(QString,QString,AlkDatePriceMap)), &multiReceiver, SLOT(quotes(QString,QString,AlkDatePriceMap)));
    QVERIFY(p.parseQuoteJson(quotedata));

    AlkOnlineQuoteSource source2("test", "", "", AlkOnlineQuoteSource::Symbol, "chart:result:0:indicators:quote:open", "chart:result:0:timestamp", "%u", AlkOnlineQuoteSource::JSON);
    p.m_source = source;
    QVERIFY(p.parseQuoteJson(quotedata));
#endif
}

#include "alkonlinequoteprivatetest.moc"
