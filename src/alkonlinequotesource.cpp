/***************************************************************************
 *   Copyright 2018  Ralf Habacker <ralf.habacker@freenet.de>              *
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

#include "alkonlinequotesource.h"

#include <KStandardDirs>
#include <KConfig>
#include <KConfigGroup>

class AlkOnlineQuoteSource::Private
{
public:
  Private() : m_skipStripping(false) {}
  Private(const Private* other)
    : m_name(other->m_name),
      m_url(other->m_url),
      m_sym(other->m_sym),
      m_price(other->m_price),
      m_date(other->m_date),
      m_dateformat(other->m_dateformat),
      m_skipStripping(other->m_skipStripping)
  {}

  bool read(const QString &groupName)
  {
    const QString &group = QString("Online-Quote-Source-%1").arg(groupName);
    KSharedConfigPtr kconfig = KGlobal::config();
    if (!kconfig->hasGroup(group))
      return false;
    KConfigGroup grp = kconfig->group(group);
    m_name = groupName;
    m_sym = grp.readEntry("SymbolRegex");
    m_date = grp.readEntry("DateRegex");
    m_dateformat = grp.readEntry("DateFormatRegex", "%m %d %y");
    m_price = grp.readEntry("PriceRegex");
    m_url = grp.readEntry("URL");
    m_skipStripping = grp.readEntry("SkipStripping", false);
    return true;
  }

  bool write(const QString &groupName)
  {
    KSharedConfigPtr kconfig = KGlobal::config();
    KConfigGroup grp = kconfig->group(QString("Online-Quote-Source-%1").arg(groupName));
    grp.writeEntry("URL", m_url);
    grp.writeEntry("PriceRegex", m_price);
    grp.writeEntry("DateRegex", m_date);
    grp.writeEntry("DateFormatRegex", m_dateformat);
    grp.writeEntry("SymbolRegex", m_sym);
    if (m_skipStripping)
      grp.writeEntry("SkipStripping", m_skipStripping);
    else
      grp.deleteEntry("SkipStripping");
    kconfig->sync();
    return true;
  }

  // This is currently in skrooge format
  bool readFromGHNSFile(const QString &relPath, const QString &name)
  {
    QString configFile = KStandardDirs::locate("data", relPath + name);
    if (configFile.isEmpty())
      return false;
    KConfig config(configFile);
    KConfigGroup group(&config, "<default>");
    if (!(group.hasKey("mode") && group.readEntry("mode") == "HTML" &&
      group.hasKey("url") && group.hasKey("date") &&
      group.hasKey("dateformat") && group.hasKey("price")))
       return false;
    m_skipStripping = false;
    m_name = name;
    m_url = group.readEntry("url");
    m_price = group.readEntry("price");
    m_date = group.readEntry("date");
    m_dateformat = group.readEntry("dateformat");
    return true;
  }

  // This is currently in skrooge format
  bool writeToGHNSFile(const QString &relPath, const QString &name)
  {
    QString configFile = KStandardDirs::locateLocal("data", relPath + name);
    if (configFile.isEmpty())
      return false;
    KConfig config(configFile);
    KConfigGroup group(&config, "<default>");
    group.writeEntry("url", m_url);
    group.writeEntry("price", m_price);
    group.writeEntry("date", m_date);
    group.writeEntry("dateformat", m_dateformat);
    group.writeEntry("mode", "HTML");
    return true;
  }

  QString    m_name;
  QString    m_url;
  QString    m_sym;
  QString    m_price;
  QString    m_date;
  QString    m_dateformat;
  bool       m_skipStripping;
};

AlkOnlineQuoteSource::AlkOnlineQuoteSource()
  : d(new Private)
{
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const AlkOnlineQuoteSource &other)
 : d(new Private(other.d))
{
}

AlkOnlineQuoteSource &AlkOnlineQuoteSource::operator=(const AlkOnlineQuoteSource &other)
{
  delete d;
  d = new Private(other.d);
  return *this;
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const QString& name, const QString& url, const QString& sym, const QString& price, const QString& date, const QString& dateformat, bool skipStripping)
  : d(new Private)
{
  d->m_name = name;
  d->m_url = url;
  d->m_sym = sym;
  d->m_price = price;
  d->m_date = date;
  d->m_dateformat = dateformat;
  d->m_skipStripping = skipStripping;
}

AlkOnlineQuoteSource::AlkOnlineQuoteSource(const QString& name)
  : d(new Private)
{
  if (name.endsWith(".txt")) {
    d->readFromGHNSFile("skrooge/quotes/", name);
  } else {
    d->read(name);
  }
}

AlkOnlineQuoteSource::~AlkOnlineQuoteSource()
{
  delete d;
}

bool AlkOnlineQuoteSource::isValid()
{
  return !d->m_name.isEmpty();
}

QString AlkOnlineQuoteSource::name() const
{
  return d->m_name;
}

QString AlkOnlineQuoteSource::url() const
{
  return d->m_url;
}

QString AlkOnlineQuoteSource::sym() const
{
  return d->m_sym;
}

QString AlkOnlineQuoteSource::price() const
{
  return d->m_price;
}

QString AlkOnlineQuoteSource::date() const
{
  return d->m_date;
}

QString AlkOnlineQuoteSource::dateformat() const
{
  return d->m_dateformat;
}

bool AlkOnlineQuoteSource::skipStripping() const
{
  return d->m_skipStripping;
}

void AlkOnlineQuoteSource::setName(const QString &name)
{
  d->m_name = name;
}

void AlkOnlineQuoteSource::setUrl(const QString &url)
{
  d->m_url = url;
}

void AlkOnlineQuoteSource::setSym(const QString &symbol)
{
  d->m_sym = symbol;
}

void AlkOnlineQuoteSource::setPrice(const QString &price)
{
  d->m_price = price;
}

void AlkOnlineQuoteSource::setDate(const QString &date)
{
  d->m_date = date;
}

void AlkOnlineQuoteSource::setDateformat(const QString &dateformat)
{
  d->m_dateformat = dateformat;
}

void AlkOnlineQuoteSource::setSkipStripping(bool state)
{
  d->m_skipStripping = state;
}

bool AlkOnlineQuoteSource::write() const
{
  // FIXME
  if (d->m_name.endsWith(".txt")) {
    return d->writeToGHNSFile("skrooge/quotes/", d->m_name);
  } else {
    return d->write(d->m_name);
  }
}

void AlkOnlineQuoteSource::rename(const QString& name)
{
  remove();
  d->m_name = name;
  write();
}

void AlkOnlineQuoteSource::remove() const
{
  KSharedConfigPtr kconfig = KGlobal::config();
  kconfig->deleteGroup(QString("Online-Quote-Source-%1").arg(d->m_name));
  kconfig->sync();
}
