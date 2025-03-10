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

#include "alkdownloadengine.h"
#include "alkonlinequoteprocess.h"
#include "alkonlinequotesource.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#define KUrl QUrl
#define prettyUrl() toDisplayString()
#include <QUrl>
#else
#include <KUrl>
#endif
class QNetworkReply;

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif
#include <QTimer>

class ALK_EXPORT AlkOnlineQuote::Private : public QObject
{
    Q_OBJECT
public:
    AlkOnlineQuote *m_p;
    AlkOnlineQuoteProcess m_filter;
    QString m_quoteData;
    QString m_symbol;
    QStringList m_symbols;
    QString m_id;
    QDate m_startDate;
    QDate m_endDate;
    QDate m_date;
    double m_price;
    AlkOnlineQuoteSource m_source;
    AlkOnlineQuote::Errors m_errors;
    AlkDownloadEngine m_downloader;
    KUrl m_url;
    QEventLoop *m_eventLoop;
    QString m_acceptLanguage;
    AlkOnlineQuotesProfile *m_profile;
    bool m_ownProfile;
    int m_timeout;
    bool m_useSingleQuoteSignal{false};
    LastPriceState m_alwaysReturnLastPrice{AlwaysWhenToday};
    bool m_reverseLaunchEnabled{true};
    bool m_reverseLaunch{false};

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static int dbgArea();
#endif

    explicit Private(AlkOnlineQuote *parent);

    ~Private();

    /**
     * Apply date range to specified URL
     * @param url Reference of the URL to which the date range is to be applied
     * @return true the date range was applied to the specified URL
     * @return false there was an error applying the date range; the URL was not changed
     */
    bool applyDateRange(QUrl &url);
    bool initSource(const QString &_source);
    bool initLaunch(const QString &_symbol, const QString &_id);
    bool launch(const QString &symbol, const QString &id, const QString &source);
    bool launchWithJavaScriptSupport(const QString &_symbol, const QString &_id, AlkDownloadEngine::Type type);
    bool launchNative(const QString &_symbol, const QString &_id);
    bool launchFinanceQuote(const QString& _symbol, const QString& _id, const QString& _source);
    bool parsePrice(const QString &pricestr, AlkOnlineQuoteSource::DecimalSeparator separator = AlkOnlineQuoteSource::Legacy);
    bool parseDate(const QString &datestr);
    bool parseQuoteStripHTML(const QString &quotedata);
    bool parseQuoteHTML(const QString &quotedata);
    void parseQuoteCSS(AlkWebPage *page);
    bool parseQuoteCSV(const QString &quotedata);
    bool parseQuoteJson(const QString &quotedata);
    bool processDownloadedPage(const KUrl &url, const QByteArray &page);
    bool processLocalScript(const KUrl& url);

    /**
     * Return of a subtree within the tree passed with \p data based on a hierarchy specification with \p path
     * The hierarchy of the specified tree can consist of instances of QVariantMap or QVariantList, which are
     * identified via corresponding parts of the path specified with \o path, with a key for QVariantMap
     * and an index for QVariantList. If no index is specified, the first entry is used.
     * See the examples for a hierachy setup as:
     *  \code{.cpp}
     *     QVariantMap["key1"]->QVariantMap["key2"]->QVariantMap["key3"]  -> path = "key1:key2:key3"
     *     QVariantMap["key1"]->QVariantList[n]->QVariantMap["key2"] -> path = "key1:<0..n-1>:key2" or "key1:key2" for the use of the first entry for compatibility reasons
     *  \endcode
     * @param data tree to be scanned
     * @param path hierarchy to be searched for in the form “<key>[:<key>]...” or “<key>[:<index>][:<key>]”
     * @param resultData return variable for the sub-tree found
     * @param errorKey return variable for the actual processed key in the event of an error
     * @return true - success, the path has been found and the associated sub-tree returned
     * @return false - error when searching for the specified path
     */
    static bool getSubTree(const QVariantMap &data, const QString &path, QVariant &resultData, QString &errorKey);

public Q_SLOTS:
    void slotLoadError(const QUrl &, const QString &);
    void slotLoadFinished(const QUrl &url, const QString &data);
    void slotLoadFinishedPage(const QUrl &, AlkWebPage *page);
    void slotLoadRedirected(const QUrl &, const QUrl &);
    void slotLoadStarted(const QUrl &);
    void slotLoadTimeout(const QUrl &);
    bool slotParseQuote(const QString &quotedata);
};

#endif // ALKONLINEQUOTE_PRIVATE_H
