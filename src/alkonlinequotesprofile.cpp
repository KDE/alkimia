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

#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"

#include <QString>
#include <QtDebug>

#include <KStandardDirs>
#include <knewstuff3/downloadmanager.h>

class AlkOnlineQuotesProfile::Private : public QObject
{
  Q_OBJECT
public:
  AlkOnlineQuotesProfile *m_p;
  QString m_name;
  QString m_GHNSFile;
  QString m_GHNSFilePath;
  QString m_kconfigFile;
  AlkOnlineQuotesProfileManager *m_profileManager;
  KNS3::DownloadManager *m_manager;
  KConfig *m_config;

  Private(AlkOnlineQuotesProfile *p)
    : m_p(p),
      m_profileManager(0),
      m_manager(0),
      m_config(0)
  {
  }

  ~Private()
  {
    delete m_manager;
    delete m_config;
  }

  void checkUpdates()
  {
    m_manager = new KNS3::DownloadManager(m_p->hotNewStuffConfigFile(), this);
    // to know when checking for updates is done
    connect(m_manager, SIGNAL(searchResult(KNS3::Entry::List)), this, SLOT(slotUpdatesFound(KNS3::Entry::List)));
    // to know about finished installations
    connect(m_manager, SIGNAL(entryStatusChanged(KNS3::Entry)), this, SLOT(entryStatusChanged(KNS3::Entry)));
    // start checking for updates
    m_manager->checkForUpdates();
  }

public Q_SLOTS:
  void slotUpdatesFound(const KNS3::Entry::List &updates)
  {
    foreach (const KNS3::Entry& entry, updates) {
      qDebug() << entry.name();
    }
  }

  // to know about finished installations
  void entryStatusChanged(const KNS3::Entry &entry)
  {
    qDebug() << entry.summary();
  }
};

AlkOnlineQuotesProfile::AlkOnlineQuotesProfile(const QString &name, Type type, const QString &configFile)
  : d(new Private(this))
{
  d->m_name = name;
  d->m_GHNSFile = configFile;
  if (type == Type::GHNS) {
      // TODO read file
      d->m_GHNSFilePath = "skrooge/quotes";
  }
  d->m_kconfigFile = name + "rc";
  d->m_config = new KConfig(d->m_kconfigFile);
  d->checkUpdates();
}

AlkOnlineQuotesProfile::~AlkOnlineQuotesProfile()
{
  delete d;
}

QString AlkOnlineQuotesProfile::name() const
{
  return d->m_name;
}

QString AlkOnlineQuotesProfile::hotNewStuffConfigFile() const
{
    QString configFile = KStandardDirs::locate("config", d->m_GHNSFile);
    if (configFile.isEmpty())
        configFile = QString("%1/%2").arg(KNSRC_DIR, d->m_GHNSFile);

  return configFile;
}

QString AlkOnlineQuotesProfile::hotNewStuffReadFilePath(const QString &fileName) const
{
    return KStandardDirs::locate("data", d->m_GHNSFilePath + "/" +  fileName);
}

QString AlkOnlineQuotesProfile::hotNewStuffWriteFilePath(const QString &fileName) const
{
    return KStandardDirs::locateLocal("data", d->m_GHNSFilePath + "/" + fileName);
}

QString AlkOnlineQuotesProfile::hotNewStuffRelPath() const
{
  return d->m_GHNSFilePath;
}

QString AlkOnlineQuotesProfile::kConfigFile() const
{
    return d->m_kconfigFile;
}

KConfig *AlkOnlineQuotesProfile::kConfig() const
{
    return d->m_config;
}

void AlkOnlineQuotesProfile::setManager(AlkOnlineQuotesProfileManager *manager)
{
    d->m_profileManager = manager;
}

AlkOnlineQuotesProfileManager *AlkOnlineQuotesProfile::manager()
{
    return d->m_profileManager;
}

#include "alkonlinequotesprofile.moc"
