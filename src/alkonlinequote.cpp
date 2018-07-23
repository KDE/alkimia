/***************************************************************************
 *   Copyright 2004  Ace Jones <acejones@users.sourceforge.net>            *
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

#include "alkonlinequoteprocess.h"
#include "alkfinancequoteprocess.h"
#include "alkdateformat.h"
#include "alkexception.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QtWebKit>
#include <QTextStream>
#include <QTextCodec>
#include <QByteArray>

#include <KConfigGroup>
#include <KDebug>
#include <KEncodingProber>
#include <KGlobal>
#include <kio/netaccess.h>
#include <kio/scheduler.h>
#include <KLocale>
#include <KProcess>
#include <KShell>
#include <KStandardDirs>

// define static members
QString AlkOnlineQuote::m_financeQuoteScriptPath;
QStringList AlkOnlineQuote::m_financeQuoteSources;

class AlkOnlineQuote::Private
{
public:
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
  QWebView *m_webView;

  static int dbgArea() {
    static int s_area = KDebug::registerArea("KMyMoney (WebPriceQuote)");
    return s_area;
  }
};

AlkOnlineQuote::AlkOnlineQuote(QObject* _parent):
    QObject(_parent),
    d(new Private)
{
  // only do this once (I know, it is not thread safe, but it should
  // always yield the same result so we don't do any semaphore foo here)
  if (m_financeQuoteScriptPath.isEmpty()) {
    m_financeQuoteScriptPath = KGlobal::dirs()->findResource("appdata",
                               QString("misc/financequote.pl"));
  }
  connect(&d->m_filter, SIGNAL(processExited(QString)), this, SLOT(slotParseQuote(QString)));
}

AlkOnlineQuote::~AlkOnlineQuote()
{
  delete d;
}

bool AlkOnlineQuote::launch(const QString& _symbol, const QString& _id, const QString& _source)
{
  if (_source.contains("Finance::Quote"))
    return launchFinanceQuote(_symbol, _id, _source);
  else if (_source.endsWith(".css"))
    return launchWebKitCssSelector(_symbol, _id, _source);
  else if (_source.endsWith(".kio"))
    return launchNative(_symbol, _id, _source);
  else
    return launchWebKitHtmlParser(_symbol, _id, _source);
}

const AlkOnlineQuote::Errors &AlkOnlineQuote::errors()
{
    return d->m_errors;
}

bool AlkOnlineQuote::initLaunch(const QString& _symbol, const QString& _id, const QString& _source)
{
  d->m_symbol = _symbol;
  d->m_id = _id;
  d->m_errors = Errors::None;

  emit status(QString("(Debug) symbol=%1 id=%2...").arg(_symbol,_id));

  // Get sources from the config file
  QString source = _source;
  if (source.isEmpty())
    source = "KMyMoney Currency";

  if (!quoteSources().contains(source)) {
    emit error(i18n("Source <placeholder>%1</placeholder> does not exist.", source));
    d->m_errors |= Errors::Source;
    return false;
  }

  d->m_source = AlkOnlineQuoteSource(source);

  KUrl url;

  // if the source has room for TWO symbols..
  if (d->m_source.url().contains("%2")) {
    // this is a two-symbol quote.  split the symbol into two.  valid symbol
    // characters are: 0-9, A-Z and the dot.  anything else is a separator
    QRegExp splitrx("([0-9a-z\\.]+)[^a-z0-9]+([0-9a-z\\.]+)", Qt::CaseInsensitive);
    // if we've truly found 2 symbols delimited this way...
    if (splitrx.indexIn(d->m_symbol) != -1)
      url = KUrl(d->m_source.url().arg(splitrx.cap(1), splitrx.cap(2)));
    else
      kDebug(Private::dbgArea()) << "WebPriceQuote::launch() did not find 2 symbols";
  } else
    // a regular one-symbol quote
    url = KUrl(d->m_source.url().arg(d->m_symbol));

  d->m_url = url;

  return true;
}

void AlkOnlineQuote::slotLoadFinishedHtmlParser(bool ok)
{
  if (!ok) {
    emit error(i18n("Unable to fetch url for %1").arg(d->m_symbol));
    d->m_errors |= Errors::URL;
    emit failed(d->m_id, d->m_symbol);
  } else {
    // parse symbol
    QWebFrame *frame = d->m_webView->page()->mainFrame();
    slotParseQuote(frame->toHtml());
  }
   d->m_eventLoop->exit();
}

void AlkOnlineQuote::slotLoadFinishedCssSelector(bool ok)
{
  if (!ok) {
    emit error(i18n("Unable to fetch url for %1").arg(d->m_symbol));
    d->m_errors |= Errors::URL;
    emit failed(d->m_id, d->m_symbol);
  } else {
    // parse symbol
    QWebFrame *frame = d->m_webView->page()->mainFrame();
    QWebElement element = frame->findFirstElement(d->m_source.sym());
    QString symbol = element.toPlainText();
    if (!symbol.isEmpty()) {
      emit status(i18n("Symbol found: '%1'", symbol));
    } else {
      d->m_errors |= Errors::Symbol;
      emit error(i18n("Unable to parse symbol for %1", d->m_symbol));
    }

    // parse price
    element = frame->findFirstElement(d->m_source.price());
    QString price = element.toPlainText();
    bool gotprice = parsePrice(price);

    // parse date
    element = frame->findFirstElement(d->m_source.date());
    QString date = element.toPlainText();
    bool gotdate = parseDate(date);

    if (gotprice && gotdate) {
      emit quote(d->m_id, d->m_symbol, d->m_date, d->m_price);
    } else {
      emit failed(d->m_id, d->m_symbol);
    }
  }
  d->m_eventLoop->exit();
}

void AlkOnlineQuote::slotLoadStarted()
{
  emit status(i18n("Fetching URL %1...", d->m_url.prettyUrl()));
}

void AlkOnlineQuote::setWebView(QWebView *view)
{
    d->m_webView = view;
}

bool AlkOnlineQuote::launchWebKitCssSelector(const QString& _symbol, const QString& _id, const QString& _source)
{
  if (!initLaunch(_symbol, _id, _source))
    return false;
  connect(d->m_webView, SIGNAL(loadStarted(bool)), this, SLOT(slotLoadStarted(bool)));
  connect(d->m_webView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedCssSelector(bool)));
  d->m_webView->setUrl(d->m_url);
  d->m_eventLoop = new QEventLoop;
  d->m_eventLoop->exec();
  delete d->m_eventLoop;
  delete d->m_webView;
  disconnect(d->m_webView, SIGNAL(loadStarted(bool)), this, SLOT(slotLoadStarted(bool)));
  disconnect(d->m_webView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedCssSelector(bool)));

  return !(d->m_errors & Errors::URL || d->m_errors & Errors::Price || d->m_errors & Errors::Date || d->m_errors & Errors::Data);
}

bool AlkOnlineQuote::launchWebKitHtmlParser(const QString& _symbol, const QString& _id, const QString& _source)
{
  if (!initLaunch(_symbol, _id, _source))
    return false;
  connect(d->m_webView, SIGNAL(loadStarted()), this, SLOT(slotLoadStarted()));
  connect(d->m_webView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedHtmlParser(bool)));
  d->m_webView->setUrl(d->m_url);
  d->m_eventLoop = new QEventLoop;
  d->m_eventLoop->exec();
  delete d->m_eventLoop;
  disconnect(d->m_webView, SIGNAL(loadStarted(bool)), this, SLOT(slotLoadStarted(bool)));
  disconnect(d->m_webView, SIGNAL(loadFinished(bool)), this, SLOT(slotLoadFinishedHtmlParser(bool)));

  return !(d->m_errors & Errors::URL || d->m_errors & Errors::Price || d->m_errors & Errors::Date || d->m_errors & Errors::Data);
}

bool AlkOnlineQuote::launchNative(const QString& _symbol, const QString& _id, const QString& _source)
{
  bool result = true;
  if (!initLaunch(_symbol, _id, _source))
    return false;

  KUrl url = d->m_url;
  if (url.isLocalFile()) {
    emit status(i18nc("The process x is executing", "Executing %1...", url.toLocalFile()));

    d->m_filter.clearProgram();
    d->m_filter << url.toLocalFile().split(' ', QString::SkipEmptyParts);
    d->m_filter.setSymbol(d->m_symbol);

    d->m_filter.setOutputChannelMode(KProcess::MergedChannels);
    d->m_filter.start();

    if (d->m_filter.waitForStarted()) {
      result = true;
    } else {
      emit error(i18n("Unable to launch: %1", url.toLocalFile()));
      d->m_errors |= Errors::Script;
      result = slotParseQuote(QString());
    }
  } else {
    slotLoadStarted();

    QString tmpFile;
    if (KIO::NetAccess::download(url, tmpFile, 0)) {
      // kDebug(Private::dbgArea()) << "Downloaded " << tmpFile;
      kDebug(Private::dbgArea()) << "Downloaded" << tmpFile << "from" << url;
      QFile f(tmpFile);
      if (f.open(QIODevice::ReadOnly)) {
        // Find out the page encoding and convert it to unicode
        QByteArray page = f.readAll();
        KEncodingProber prober(KEncodingProber::Universal);
        prober.feed(page);
        QTextCodec* codec = QTextCodec::codecForName(prober.encoding());
        if (!codec)
          codec = QTextCodec::codecForLocale();
        QString quote = codec->toUnicode(page);
        f.close();
        emit status(i18n("URL found: %1...", url.prettyUrl()));
        result = slotParseQuote(quote);
      } else {
        emit error(i18n("Failed to open downloaded file"));
        d->m_errors |= Errors::URL;
        result = slotParseQuote(QString());
      }
      KIO::NetAccess::removeTempFile(tmpFile);
    } else {
      emit error(KIO::NetAccess::lastErrorString());
      d->m_errors |= Errors::URL;
      result = slotParseQuote(QString());
    }
  }
  return result;
}

bool AlkOnlineQuote::launchFinanceQuote(const QString& _symbol, const QString& _id,
                                       const QString& _sourcename)
{
  bool result = true;
  d->m_symbol = _symbol;
  d->m_id = _id;
  d->m_errors = Errors::None;

  QString FQSource = _sourcename.section(' ', 1);
  d->m_source = AlkOnlineQuoteSource(_sourcename, m_financeQuoteScriptPath,
                                    "\"([^,\"]*)\",.*",  // symbol regexp
                                    "[^,]*,[^,]*,\"([^\"]*)\"", // price regexp
                                    "[^,]*,([^,]*),.*", // date regexp
                                    "%y-%m-%d"); // date format

  //emit status(QString("(Debug) symbol=%1 id=%2...").arg(_symbol,_id));

  d->m_filter.clearProgram();
  d->m_filter << "perl" << m_financeQuoteScriptPath << FQSource << KShell::quoteArg(_symbol);
  d->m_filter.setSymbol(d->m_symbol);
  emit status(i18nc("Executing 'script' 'online source' 'investment symbol' ", "Executing %1 %2 %3...", m_financeQuoteScriptPath, FQSource, _symbol));

  d->m_filter.setOutputChannelMode(KProcess::MergedChannels);
  d->m_filter.start();

  // This seems to work best if we just block until done.
  if (d->m_filter.waitForFinished()) {
  } else {
    emit error(i18n("Unable to launch: %1", m_financeQuoteScriptPath));
    d->m_errors |= Errors::Script;
    result = slotParseQuote(QString());
  }

  return result;
}

bool AlkOnlineQuote::parsePrice(const QString &_pricestr)
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

    d->m_price = pricestr.toDouble();
    kDebug(Private::dbgArea()) << "Price" << pricestr;
    emit status(i18n("Price found: '%1' (%2)", pricestr, d->m_price));
  } else {
    d->m_errors |= Errors::Price;
    emit error(i18n("Unable to parse price for %1", d->m_symbol));
    result = false;
  }
  return result;
}

bool AlkOnlineQuote::parseDate(const QString &datestr)
{
  if (!datestr.isEmpty()) {
    emit status(i18n("Date found: '%1'", datestr));

    AlkDateFormat dateparse(d->m_source.dateformat());
    try {
      d->m_date = dateparse.convertString(datestr, false /*strict*/);
      kDebug(Private::dbgArea()) << "Date" << datestr;
      emit status(i18n("Date format found: '%1' -> %2", datestr, d->m_date.toString()));
    } catch (const AlkException &e) {
      d->m_errors |= Errors::DateFormat;
      emit error(i18n("Unable to parse date %1 using format %2: %3").arg(datestr,dateparse.format(), e.what()));
      d->m_date = QDate::currentDate();
      emit status(i18n("Using current date for %1").arg(d->m_symbol));
    }
  } else {
    d->m_errors |= Errors::Date;
    emit error(i18n("Unable to parse date for %1").arg(d->m_symbol));
    d->m_date = QDate::currentDate();
    emit status(i18n("Using current date for %1").arg(d->m_symbol));
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
bool AlkOnlineQuote::slotParseQuote(const QString& _quotedata)
{
  QString quotedata = _quotedata;
  d->m_quoteData = quotedata;
  bool gotprice = false;
  bool gotdate = false;
  bool result = true;

  kDebug(Private::dbgArea()) << "quotedata" << _quotedata;

  if (! quotedata.isEmpty()) {
    if (!d->m_source.skipStripping()) {
      //
      // First, remove extranous non-data elements
      //

      // HTML tags
      quotedata.remove(QRegExp("<[^>]*>"));

      // &...;'s
      quotedata.replace(QRegExp("&\\w+;"), " ");

      // Extra white space
      quotedata = quotedata.simplified();
      kDebug(Private::dbgArea()) << "stripped text" << quotedata;
    }

    QRegExp symbolRegExp(d->m_source.sym());
    QRegExp dateRegExp(d->m_source.date());
    QRegExp priceRegExp(d->m_source.price());

    if (symbolRegExp.indexIn(quotedata) > -1) {
      kDebug(Private::dbgArea()) << "Symbol" << symbolRegExp.cap(1);
      emit status(i18n("Symbol found: '%1'", symbolRegExp.cap(1)));
    } else {
      d->m_errors |= Errors::Symbol;
      emit error(i18n("Unable to parse symbol for %1", d->m_symbol));
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
      emit quote(d->m_id, d->m_symbol, d->m_date, d->m_price);
    } else {
      emit failed(d->m_id, d->m_symbol);
      result = false;
    }
  } else {
    d->m_errors |= Errors::Data;
    emit error(i18n("Unable to update price for %1 (empty quote data)", d->m_symbol));
    emit failed(d->m_id, d->m_symbol);
    result = false;
  }
  return result;
}

const QMap<QString, AlkOnlineQuoteSource> AlkOnlineQuote::defaultQuoteSources()
{
  QMap<QString, AlkOnlineQuoteSource> result;

  // Use fx-rate.net as the standard currency exchange rate source until
  // we have the capability to use more than one source. Use a neutral
  // name for the source.

  if (AlkOnlineQuoteSource::profile()->name() == "alkimia") {
    result["Alkimia Currency"] =
      AlkOnlineQuoteSource("Alkimia Currency",
                           "https://fx-rate.net/%1/%2",
                            QString(),  // symbolregexp
                           "1[ a-zA-Z]+=</span><br */?> *(\\d+\\.\\d+)",
                           "updated\\s\\d+:\\d+:\\d+\\(\\w+\\)\\s+(\\d{1,2}/\\d{2}/\\d{4})",
                           "%d/%m/%y",
                           true // skip HTML stripping
                          );
  }
  return result;
}

const QStringList AlkOnlineQuote::quoteSources(const _quoteSystemE _system)
{
  QStringList result;
  if (_system == Native)
    result << quoteSourcesNative();
  else
    result << quoteSourcesFinanceQuote();
  result << quoteSourcesSkrooge();
  return result;
}

const QStringList AlkOnlineQuote::quoteSourcesNative()
{
  //KSharedConfigPtr kconfig = KGlobal::config();
  KConfig config(AlkOnlineQuoteSource::profile()->kConfigFile());
  KConfig *kconfig = &config;
  QStringList groups = kconfig->groupList();

  QStringList::Iterator it;
  QRegExp onlineQuoteSource(QString("^Online-Quote-Source-(.*)$"));

  // get rid of all 'non online quote source' entries
  for (it = groups.begin(); it != groups.end(); it = groups.erase(it)) {
    if (onlineQuoteSource.indexIn(*it) >= 0) {
      // Insert the name part
      it = groups.insert(it, onlineQuoteSource.cap(1));
      ++it;
    }
  }

  // Set up each of the default sources.  These are done piecemeal so that
  // when we add a new source, it's automatically picked up. And any changes
  // are also picked up.
  QMap<QString, AlkOnlineQuoteSource> defaults = defaultQuoteSources();
  QMap<QString, AlkOnlineQuoteSource>::const_iterator it_source = defaults.constBegin();
  while (it_source != defaults.constEnd()) {
    if (! groups.contains((*it_source).name())) {
      groups += (*it_source).name();
      (*it_source).write();
      kconfig->sync();
    }
    ++it_source;
  }

  return groups;
}

const QStringList AlkOnlineQuote::quoteSourcesFinanceQuote()
{
  if (m_financeQuoteSources.empty()) { // run the process one time only
    // since this is a static function it can be called without constructing an object
    // so we need to make sure that m_financeQuoteScriptPath is properly initialized
    if (m_financeQuoteScriptPath.isEmpty()) {
      m_financeQuoteScriptPath = KGlobal::dirs()->findResource("appdata",
                                 QString("financequote.pl"));
    }
    AlkFinanceQuoteProcess getList;
    getList.launch(m_financeQuoteScriptPath);
    while (!getList.isFinished()) {
      qApp->processEvents();
    }
    m_financeQuoteSources = getList.getSourceList();
  }
  return (m_financeQuoteSources);
}

const QStringList AlkOnlineQuote::quoteSourcesSkrooge()
{
  QStringList sources;
  QString relPath = AlkOnlineQuoteSource::profile()->hotNewStuffRelPath();

  foreach(const QString &file, KStandardDirs().findAllResources("data", relPath + QString::fromLatin1("/*.txt"))) {
    QFileInfo f(file);
    QString file2 = f.fileName();
    if (!sources.contains(file2)) {
      sources.push_back(file2);
    }
  }

  return sources;
}
