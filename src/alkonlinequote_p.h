/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKONLINEQUOTE_PRIVATE_H
#define ALKONLINEQUOTE_PRIVATE_H

#include "alkonlinequote.h"

#ifdef ENABLE_FINANCEQUOTE
#include "alkonlinequoteprocess.h"
#endif
#include "alkonlinequotesource.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #define KUrl QUrl
    #define prettyUrl() toDisplayString()
    #include <QUrl>
#else
    #include <KUrl>
#endif
#ifdef BUILD_WITH_QTNETWORK
class QNetworkReply;
#else
class KJob;
#endif

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QTimer>

class AlkOnlineQuote::Private : public QObject
{
    Q_OBJECT
public:
    AlkOnlineQuote *m_p;
#ifdef ENABLE_FINANCEQUOTE
    AlkOnlineQuoteProcess m_filter;
#endif
    QString m_quoteData;
    QString m_symbol;
    QString m_id;
    QDate m_startDate;
    QDate m_endDate;
    QDate m_date;
    double m_price;
    AlkOnlineQuoteSource m_source;
    AlkOnlineQuote::Errors m_errors;
    KUrl m_url;
    QEventLoop *m_eventLoop;
    QString m_acceptLanguage;
    AlkOnlineQuotesProfile *m_profile;
    bool m_ownProfile;
    int m_timeout;

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static int dbgArea();
#endif

    explicit Private(AlkOnlineQuote *parent);

    ~Private();

    bool initLaunch(const QString &_symbol, const QString &_id, const QString &_source);
    bool launchWebKitCssSelector(const QString &_symbol, const QString &_id,
                                 const QString &_source);
    bool launchWebKitHtmlParser(const QString &_symbol, const QString &_id, const QString &_source);
    bool launchNative(const QString &_symbol, const QString &_id, const QString &_source);
    bool launchFinanceQuote(const QString& _symbol, const QString& _id, const QString& _source);
    bool parsePrice(const QString &pricestr);
    bool parseDate(const QString &datestr);
    bool parseQuoteStripHTML(const QString &quotedata);
    bool parseQuoteHTML(const QString &quotedata);
    bool parseQuoteCSV(const QString &quotedata);
    bool downloadUrl(const KUrl& url);
    bool processDownloadedFile(const KUrl& url, const QString& tmpFile);
    bool processDownloadedPage(const KUrl &url, const QByteArray &page);
#ifdef ENABLE_FINANCEQUOTE
    bool processLocalScript(const KUrl& url);
#endif

public Q_SLOTS:
    void slotLoadStarted();
    void slotLoadFinishedHtmlParser(bool ok = false);
    void slotLoadFinishedCssSelector(bool ok);
    bool slotParseQuote(const QString &quotedata);

private Q_SLOTS:
    void slotLoadTimeout();
#ifndef BUILD_WITH_QTNETWORK
    void downloadUrlDone(KJob* job);
#else
    void downloadUrlDone(QNetworkReply *reply);
#endif
};

#endif // ALKONLINEQUOTE_PRIVATE_H
