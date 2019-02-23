/***************************************************************************
 *   Copyright 2004  Ace Jones <acejones@users.sourceforge.net>            *
 *   Copyright 2019  Thomas Baumgart <tbaumgart@kde.org>                   *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#include "alkonlinequote.h"

#include "alkdateformat.h"
#include "alkexception.h"
#include "alkfinancequoteprocess.h"
#include "alkonlinequoteprocess.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"
#include "alkwebpage.h"

#include <QApplication>
#include <QByteArray>
#include <QFile>
#include <QRegExp>
#include <QTextStream>
#include <QTextCodec>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <KLocalizedString>
    #include <KIO/Scheduler>
    #include <KIO/Job>
    #include <QDebug>
    #include <QTemporaryFile>
    #define kDebug(a) qDebug()
    #define KIcon QIcon
    #define KUrl QUrl
    #define prettyUrl() toDisplayString()
#else
    #include <KDebug>
    #include <KGlobal>
    #include <KLocale>
    #include <KUrl>
    #include <kio/netaccess.h>
    #include <kio/scheduler.h>
#endif

#include <KConfigGroup>
#include <KEncodingProber>
#include <KProcess>
#include <KShell>

AlkOnlineQuote::Errors::Errors()
{
}

AlkOnlineQuote::Errors::Errors(Type type)
{
    m_type.append(type);
}

AlkOnlineQuote::Errors::Errors(const Errors &e)
{
    m_type = e.m_type;
}

AlkOnlineQuote::Errors &AlkOnlineQuote::Errors::operator |=(Type t)
{
    if (!m_type.contains(t)) {
        m_type.append(t);
    }
    return *this;
}

bool AlkOnlineQuote::Errors::operator &(Type t) const
{
    return m_type.contains(t);
}


class AlkOnlineQuote::Private : public QObject
{
    Q_OBJECT
public:
    AlkOnlineQuote *m_p;
    AlkOnlineQuoteProcess m_filter;
    QString m_quoteData;
    QString m_symbol;
    QString m_id;
    QDate m_date;
    double m_price;
    AlkOnlineQuoteSource m_source;
    AlkOnlineQuote::Errors m_errors;
    KUrl m_url;
    QEventLoop *m_eventLoop;
    QString m_acceptLanguage;
    AlkOnlineQuotesProfile *m_profile;
    bool m_ownProfile;

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static int dbgArea()
    {
        static int s_area = KDebug::registerArea("Alkimia (AlkOnlineQuote)");
        return s_area;
    }
#endif

    Private(AlkOnlineQuote *parent)
        : m_p(parent)
        , m_eventLoop(nullptr)
        , m_ownProfile(false)
    {
        connect(&m_filter, SIGNAL(processExited(QString)), this, SLOT(slotParseQuote(QString)));
    }

    ~Private()
    {
        if (m_ownProfile)
            delete m_profile;
    }

    bool initLaunch(const QString &_symbol, const QString &_id, const QString &_source);
    bool launchWebKitCssSelector(const QString &_symbol, const QString &_id,
                                 const QString &_source);
    bool launchWebKitHtmlParser(const QString &_symbol, const QString &_id, const QString &_source);
    bool launchNative(const QString &_symbol, const QString &_id, const QString &_source);
    bool launchFinanceQuote(const QString &_symbol, const QString &_id, const QString &_source);
    void enter_loop();
    bool parsePrice(const QString &pricestr);
    bool parseDate(const QString &datestr);
    bool downloadUrl(const KUrl& url);
    bool processDownloadedFile(const KUrl& url, const QString& tmpFile);

public slots:
    void slotLoadStarted();
    void slotLoadFinishedHtmlParser(bool ok);
    void slotLoadFinishedCssSelector(bool ok);
    bool slotParseQuote(const QString &_quotedata);

private slots:
    void downloadUrlDone(KJob* job);
};

bool AlkOnlineQuote::Private::initLaunch(const QString &_symbol, const QString &_id, const QString &_source)
{
    m_symbol = _symbol;
    m_id = _id;
    m_errors = Errors::None;

    emit m_p->status(QString("(Debug) symbol=%1 id=%2...").arg(_symbol, _id));

    // Get sources from the config file
    QString source = _source;
    if (source.isEmpty()) {
        source = "KMyMoney Currency";
    }

    if (!m_profile->quoteSources().contains(source)) {
        emit m_p->error(i18n("Source <placeholder>%1</placeholder> does not exist.", source));
        m_errors |= Errors::Source;
        return false;
    }

    //m_profile->createSource(source);
    m_source = AlkOnlineQuoteSource(source, m_profile);

    KUrl url;

    // if the source has room for TWO symbols..
    if (m_source.url().contains("%2")) {
        // this is a two-symbol quote.  split the symbol into two.  valid symbol
        // characters are: 0-9, A-Z and the dot.  anything else is a separator
        QRegExp splitrx("([0-9a-z\\.]+)[^a-z0-9]+([0-9a-z\\.]+)", Qt::CaseInsensitive);
        // if we've truly found 2 symbols delimited this way...
        if (splitrx.indexIn(m_symbol) != -1) {
            url = KUrl(m_source.url().arg(splitrx.cap(1), splitrx.cap(2)));
        } else {
            kDebug(Private::dbgArea()) << "WebPriceQuote::launch() did not find 2 symbols";
        }
    } else {
        // a regular one-symbol quote
        url = KUrl(m_source.url().arg(m_symbol));
    }

    m_url = url;

    return true;
}

void AlkOnlineQuote::Private::slotLoadFinishedHtmlParser(bool ok)
{
    if (!ok) {
        emit m_p->error(i18n("Unable to fetch url for %1").arg(m_symbol));
        m_errors |= Errors::URL;
        emit m_p->failed(m_id, m_symbol);
    } else {
        // parse symbol
        slotParseQuote(AlkOnlineQuotesProfileManager::instance().webPage()->toHtml());
    }
    if (m_eventLoop)
        m_eventLoop->exit();
}

void AlkOnlineQuote::Private::slotLoadFinishedCssSelector(bool ok)
{
    if (!ok) {
        emit m_p->error(i18n("Unable to fetch url for %1").arg(m_symbol));
        m_errors |= Errors::URL;
        emit m_p->failed(m_id, m_symbol);
    } else {
        AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
        // parse symbol
        QString symbol = webPage->getFirstElement(m_source.sym());
        if (!symbol.isEmpty()) {
            emit m_p->status(i18n("Symbol found: '%1'", symbol));
        } else {
            m_errors |= Errors::Symbol;
            emit m_p->error(i18n("Unable to parse symbol for %1", m_symbol));
        }

        // parse price
        QString price = webPage->getFirstElement(m_source.price());
        bool gotprice = parsePrice(price);

        // parse date
        QString date = webPage->getFirstElement(m_source.date());
        bool gotdate = parseDate(date);

        if (gotprice && gotdate) {
            emit m_p->quote(m_id, m_symbol, m_date, m_price);
        } else {
            emit m_p->failed(m_id, m_symbol);
        }
    }
    if (m_eventLoop)
        m_eventLoop->exit();
}

void AlkOnlineQuote::Private::slotLoadStarted()
{
    emit m_p->status(i18n("Fetching URL %1...", m_url.prettyUrl()));
}

bool AlkOnlineQuote::Private::launchWebKitCssSelector(const QString &_symbol, const QString &_id,
                                             const QString &_source)
{
    if (!initLaunch(_symbol, _id, _source)) {
        return false;
    }
    AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
    connect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    connect(webPage, SIGNAL(loadFinished(bool)), this,
            SLOT(slotLoadFinishedCssSelector(bool)));
    webPage->setUrl(m_url);
    m_eventLoop = new QEventLoop;
    m_eventLoop->exec();
    delete m_eventLoop;
    m_eventLoop = nullptr;
    disconnect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    disconnect(webPage, SIGNAL(loadFinished(bool)), this,
               SLOT(slotLoadFinishedCssSelector(bool)));

    return !(m_errors & Errors::URL || m_errors & Errors::Price
             || m_errors & Errors::Date || m_errors & Errors::Data);
}

bool AlkOnlineQuote::Private::launchWebKitHtmlParser(const QString &_symbol, const QString &_id,
                                            const QString &_source)
{
    if (!initLaunch(_symbol, _id, _source)) {
        return false;
    }
    AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
    connect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    connect(webPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedHtmlParser(bool)));
    webPage->load(m_url, m_acceptLanguage);
    m_eventLoop = new QEventLoop;
    m_eventLoop->exec();
    delete m_eventLoop;
    m_eventLoop = nullptr;
    disconnect(webPage, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
    disconnect(webPage, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedHtmlParser(bool)));

    return !(m_errors & Errors::URL || m_errors & Errors::Price
             || m_errors & Errors::Date || m_errors & Errors::Data);
}

bool AlkOnlineQuote::Private::launchNative(const QString &_symbol, const QString &_id,
                               const QString &_source)
{
    bool result = true;
    if (!initLaunch(_symbol, _id, _source)) {
        return false;
    }

    KUrl url = m_url;
    if (url.isLocalFile()) {
        emit m_p->status(i18nc("The process x is executing", "Executing %1...", url.toLocalFile()));

        m_filter.clearProgram();
        m_filter << url.toLocalFile().split(' ', QString::SkipEmptyParts);
        m_filter.setSymbol(m_symbol);

        m_filter.setOutputChannelMode(KProcess::MergedChannels);
        m_filter.start();

        if (m_filter.waitForStarted()) {
            result = true;
        } else {
            emit m_p->error(i18n("Unable to launch: %1", url.toLocalFile()));
            m_errors |= Errors::Script;
            result = slotParseQuote(QString());
        }
    } else {
        slotLoadStarted();
        result = downloadUrl(url);
    }
    return result;
}

bool AlkOnlineQuote::Private::processDownloadedFile(const KUrl& url, const QString& tmpFile)
{
  bool result = false;

  QFile f(tmpFile);
  if (f.open(QIODevice::ReadOnly)) {
    // Find out the page encoding and convert it to unicode
    QByteArray page = f.readAll();
    KEncodingProber prober(KEncodingProber::Universal);
    prober.feed(page);
    QTextCodec *codec = QTextCodec::codecForName(prober.encoding());
    if (!codec) {
      codec = QTextCodec::codecForLocale();
    }
    QString quote = codec->toUnicode(page);
    f.close();
    emit m_p->status(i18n("URL found: %1...", url.prettyUrl()));
    if (AlkOnlineQuotesProfileManager::instance().webPageEnabled())
      AlkOnlineQuotesProfileManager::instance().webPage()->setContent(quote.toLocal8Bit());
    result = slotParseQuote(quote);
  } else {
    emit m_p->error(i18n("Failed to open downloaded file"));
    m_errors |= Errors::URL;
    result = slotParseQuote(QString());
  }
  return result;
}

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
bool AlkOnlineQuote::Private::downloadUrl(const QUrl& url)
{
    // Create a temporary filename (w/o leaving the file on the filesystem)
    // In case the file is still present, the KIO::file_copy operation cannot
    // be performed on some operating systems (Windows).
    auto tmpFile = new QTemporaryFile;
    tmpFile->open();
    auto tmpFileName = QUrl::fromLocalFile(tmpFile->fileName());
    delete tmpFile;

    m_eventLoop = new QEventLoop;
    KJob *job = KIO::file_copy(url, tmpFileName, -1, KIO::HideProgressInfo);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(downloadUrlDone(KJob*)));

    auto result = m_eventLoop->exec(QEventLoop::ExcludeUserInputEvents);
    delete m_eventLoop;
    m_eventLoop = nullptr;

    return result;
}

void AlkOnlineQuote::Private::downloadUrlDone(KJob* job)
{
  QString tmpFileName = dynamic_cast<KIO::FileCopyJob*>(job)->destUrl().toLocalFile();
  QUrl url = dynamic_cast<KIO::FileCopyJob*>(job)->srcUrl();

  bool result;
  if (!job->error()) {
    qDebug() << "Downloaded" << tmpFileName << "from" << url;
    result = processDownloadedFile(url, tmpFileName);
  } else {
    emit m_p->error(job->errorString());
    m_errors |= Errors::URL;
    result = slotParseQuote(QString());
  }
  m_eventLoop->exit(result);
}

#else

// This is simply a placeholder. It is unused but needs to be present
// to make the linker happy (since the declaration of the slot cannot
// be made dependendant on QT_VERSION with the Qt4 moc compiler.
void AlkOnlineQuote::Private::downloadUrlDone(KJob* job)
{
    Q_UNUSED(job);
}

bool AlkOnlineQuote::Private::downloadUrl(const KUrl& url)
{
    bool result = false;

    QString tmpFile;
    if (KIO::NetAccess::download(url, tmpFile, nullptr)) {
        // kDebug(Private::dbgArea()) << "Downloaded " << tmpFile;
        kDebug(Private::dbgArea()) << "Downloaded" << tmpFile << "from" << url;
        result = processDownloadedFile(url, tmpFile);
        KIO::NetAccess::removeTempFile(tmpFile);
    } else {
        emit m_p->error(KIO::NetAccess::lastErrorString());
        m_errors |= Errors::URL;
        result = slotParseQuote(QString());
    }
    return result;
}
#endif

bool AlkOnlineQuote::Private::launchFinanceQuote(const QString &_symbol, const QString &_id,
                                        const QString &_sourcename)
{
    bool result = true;
    m_symbol = _symbol;
    m_id = _id;
    m_errors = Errors::None;
#if 0
    QString FQSource = _sourcename.section(' ', 1);
    m_source = AlkOnlineQuoteSource(_sourcename, m_financeQuoteScriptPath,
                                       "\"([^,\"]*)\",.*", // symbol regexp
                                       "[^,]*,[^,]*,\"([^\"]*)\"", // price regexp
                                       "[^,]*,([^,]*),.*", // date regexp
                                       "%y-%m-%d"); // date format

    //emit status(QString("(Debug) symbol=%1 id=%2...").arg(_symbol,_id));

    m_filter.clearProgram();
    m_filter << "perl" << m_financeQuoteScriptPath << FQSource << KShell::quoteArg(_symbol);
    m_filter.setSymbol(m_symbol);
    emit m_p->status(i18nc("Executing 'script' 'online source' 'investment symbol' ",
                      "Executing %1 %2 %3...", m_financeQuoteScriptPath, FQSource, _symbol));

    m_filter.setOutputChannelMode(KProcess::MergedChannels);
    m_filter.start();

    // This seems to work best if we just block until done.
    if (m_filter.waitForFinished()) {
    } else {
        emit m_p->error(i18n("Unable to launch: %1", m_financeQuoteScriptPath));
        m_errors |= Errors::Script;
        result = slotParseQuote(QString());
    }
#else
    Q_UNUSED(_sourcename);
#if !defined(Q_CC_MSVC)
    #warning to be implemented
#endif
#endif

    return result;
}

bool AlkOnlineQuote::Private::parsePrice(const QString &_pricestr)
{
    bool result = true;
    // Deal with european quotes that come back as X.XXX,XX or XX,XXX
    //
    // We will make the assumption that ALL prices have a decimal separator.
    // So "1,000" always means 1.0, not 1000.0.
    //
    // Remove all non-digits from the price string except the last one, and
    // set the last one to a period.
    QString pricestr(_pricestr);
    if (!pricestr.isEmpty()) {
        int pos = pricestr.lastIndexOf(QRegExp("\\D"));
        if (pos > 0) {
            pricestr[pos] = '.';
            pos = pricestr.lastIndexOf(QRegExp("\\D"), pos - 1);
        }
        while (pos > 0) {
            pricestr.remove(pos, 1);
            pos = pricestr.lastIndexOf(QRegExp("\\D"), pos);
        }

        m_price = pricestr.toDouble();
        kDebug(Private::dbgArea()) << "Price" << pricestr;
        emit m_p->status(i18n("Price found: '%1' (%2)", pricestr, m_price));
    } else {
        m_errors |= Errors::Price;
        emit m_p->error(i18n("Unable to parse price for '%1'", m_symbol));
        result = false;
    }
    return result;
}

bool AlkOnlineQuote::Private::parseDate(const QString &datestr)
{
    if (!datestr.isEmpty()) {
        emit m_p->status(i18n("Date found: '%1'", datestr));

        AlkDateFormat dateparse(m_source.dateformat());
        try {
            m_date = dateparse.convertString(datestr, false /*strict*/);
            kDebug(Private::dbgArea()) << "Date" << datestr;
            emit m_p->status(i18n("Date format found: '%1' -> '%2'", datestr, m_date.toString()));
        } catch (const AlkException &e) {
            m_errors |= Errors::DateFormat;
            emit m_p->error(i18n("Unable to parse date '%1' using format '%2': %3").arg(datestr,
                                                                               dateparse.format(),
                                                                               e.what()));
            m_date = QDate::currentDate();
            emit m_p->status(i18n("Using current date for '%1'").arg(m_symbol));
        }
    } else {
        m_errors |= Errors::Date;
        emit m_p->error(i18n("Unable to parse date for '%1'").arg(m_symbol));
        m_date = QDate::currentDate();
        emit m_p->status(i18n("Using current date for '%1'").arg(m_symbol));
    }
    return true;
}

/**
 * Parse quote data according to currently selected web price quote source
 *
 * @param _quotedata quote data to parse
 * @return true parsing successful
 * @return false parsing unsuccessful
 */
bool AlkOnlineQuote::Private::slotParseQuote(const QString &_quotedata)
{
    QString quotedata = _quotedata;
    m_quoteData = quotedata;
    bool gotprice = false;
    bool gotdate = false;
    bool result = true;

    kDebug(Private::dbgArea()) << "quotedata" << _quotedata;

    if (!quotedata.isEmpty()) {
        if (!m_source.skipStripping()) {
            //
            // First, remove extraneous non-data elements
            //

            // HTML tags
            quotedata.remove(QRegExp("<[^>]*>"));

            // &...;'s
            quotedata.replace(QRegExp("&\\w+;"), " ");

            // Extra white space
            quotedata = quotedata.simplified();
            kDebug(Private::dbgArea()) << "stripped text" << quotedata;
        }

        QRegExp symbolRegExp(m_source.sym());
        QRegExp dateRegExp(m_source.date());
        QRegExp priceRegExp(m_source.price());

        if (symbolRegExp.indexIn(quotedata) > -1) {
            kDebug(Private::dbgArea()) << "Symbol" << symbolRegExp.cap(1);
            emit m_p->status(i18n("Symbol found: '%1'", symbolRegExp.cap(1)));
        } else {
            m_errors |= Errors::Symbol;
            emit m_p->error(i18n("Unable to parse symbol for %1", m_symbol));
        }

        if (priceRegExp.indexIn(quotedata) > -1) {
            gotprice = true;
            QString pricestr = priceRegExp.cap(1);
            parsePrice(pricestr);
        } else {
            parsePrice(QString());
        }

        if (dateRegExp.indexIn(quotedata) > -1) {
            gotdate = true;
            QString datestr = dateRegExp.cap(1);
            parseDate(datestr);
        } else {
            parseDate(QString());
        }

        if (gotprice && gotdate) {
            emit m_p->quote(m_id, m_symbol, m_date, m_price);
        } else {
            emit m_p->failed(m_id, m_symbol);
            result = false;
        }
    } else {
        m_errors |= Errors::Data;
        emit m_p->error(i18n("Unable to update price for %1 (empty quote data)", m_symbol));
        emit m_p->failed(m_id, m_symbol);
        result = false;
    }
    return result;
}

AlkOnlineQuote::AlkOnlineQuote(AlkOnlineQuotesProfile *profile, QObject *_parent)
    : QObject(_parent)
    , d(new Private(this))
{
    if (profile)
        d->m_profile = profile;
    else {
        d->m_profile = new AlkOnlineQuotesProfile;
        d->m_ownProfile = true;
    }
}

AlkOnlineQuote::~AlkOnlineQuote()
{
    delete d;
}

AlkOnlineQuotesProfile *AlkOnlineQuote::profile()
{
    return d->m_profile;
}

void AlkOnlineQuote::setProfile(AlkOnlineQuotesProfile *profile)
{
    if (profile && d->m_ownProfile) {
        // switching from own profile to external
        delete d->m_profile;
        d->m_ownProfile = false;
        d->m_profile = profile;

    } else if (!profile && !d->m_ownProfile) {
        // switching from external to own profile
        d->m_profile = new AlkOnlineQuotesProfile;
        d->m_ownProfile = true;

    } else if (profile) {
        // exchange external profile
        d->m_profile = profile;
    }
}

void AlkOnlineQuote::setAcceptLanguage(const QString &language)
{
    d->m_acceptLanguage = language;
}

bool AlkOnlineQuote::launch(const QString &_symbol, const QString &_id, const QString &_source)
{
    if (_source.contains("Finance::Quote")) {
        return d->launchFinanceQuote(_symbol, _id, _source);
    } else if (_source.endsWith(".css")) {
        return d->launchWebKitCssSelector(_symbol, _id, _source);
    } else if (_source.endsWith(".webkit")) {
        return d->launchWebKitHtmlParser(_symbol, _id, _source);
    } else {
        return d->launchNative(_symbol, _id, _source);
    }
}

const AlkOnlineQuote::Errors &AlkOnlineQuote::errors()
{
    return d->m_errors;
}

#include "alkonlinequote.moc"
