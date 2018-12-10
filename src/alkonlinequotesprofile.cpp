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

#include "alkonlinequotesource.h"
#include "alkfinancequoteprocess.h"

#include <QApplication>
#include <QDir>
#include <QString>
#include <QtDebug>
#include <QFileInfo>

#include <KConfig>
#include <KConfigGroup>
#include <KGlobal>
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
    Type m_type;
    static QString m_financeQuoteScriptPath;
    static QStringList m_financeQuoteSources;

    Private(AlkOnlineQuotesProfile *p)
        : m_p(p)
        , m_profileManager(0)
        , m_manager(0)
        , m_config(0)
        , m_type(Type::Undefined)
    {

        if (m_financeQuoteScriptPath.isEmpty()) {
            m_financeQuoteScriptPath = KGlobal::dirs()->findResource("appdata",
                                                                     QString("misc/financequote.pl"));
        }
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
        connect(m_manager, SIGNAL(searchResult(KNS3::Entry::List)), this,
                SLOT(slotUpdatesFound(KNS3::Entry::List)));
        // to know about finished installations
        connect(m_manager, SIGNAL(entryStatusChanged(KNS3::Entry)), this,
                SLOT(entryStatusChanged(KNS3::Entry)));
        // start checking for updates
        m_manager->checkForUpdates();
    }

public Q_SLOTS:
    void slotUpdatesFound(const KNS3::Entry::List &updates)
    {
        foreach (const KNS3::Entry &entry, updates) {
            qDebug() << entry.name();
        }
    }

    // to know about finished installations
    void entryStatusChanged(const KNS3::Entry &entry)
    {
        qDebug() << entry.summary();
    }

    const QStringList quoteSourcesNative()
    {
        //KSharedConfigPtr kconfig = KGlobal::config();
        KConfig config(m_kconfigFile);
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
        QMap<QString, AlkOnlineQuoteSource>::iterator it_source = defaults.begin();
        while (it_source != defaults.end()) {
            if (!groups.contains((*it_source).name())) {
                groups += (*it_source).name();
                (*it_source).write();
                kconfig->sync();
            }
            ++it_source;
        }

        return groups;
    }

    const QStringList quoteSourcesFinanceQuote()
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
        return m_financeQuoteSources;
    }

    const QStringList quoteSourcesSkrooge()
    {
        return quoteSourcesGHNS();
    }

    const QStringList quoteSourcesGHNS()
    {
        QStringList sources;
        QString relPath = m_GHNSFilePath;

        foreach (const QString &file,
                 KStandardDirs().findAllResources("data", relPath + QString::fromLatin1("/*.txt"))) {
            QFileInfo f(file);
            QString file2 = f.completeBaseName();
            if (!sources.contains(file2)) {
                sources.push_back(file2);
            }
        }

        return sources;
    }

    const AlkOnlineQuotesProfile::Map defaultQuoteSources()
    {
        QMap<QString, AlkOnlineQuoteSource> result;

        // Use fx-rate.net as the standard currency exchange rate source until
        // we have the capability to use more than one source. Use a neutral
        // name for the source.

        switch (m_p->type()) {
        case AlkOnlineQuotesProfile::Type::Alkimia4:
        case AlkOnlineQuotesProfile::Type::Alkimia5:
            result["Alkimia Currency"]
                = AlkOnlineQuoteSource("Alkimia Currency",
                                       "https://fx-rate.net/%1/%2",
                                       QString(), // symbolregexp
                                       "1[ a-zA-Z]+=</span><br */?> *(\\d+\\.\\d+)",
                                       "updated\\s\\d+:\\d+:\\d+\\(\\w+\\)\\s+(\\d{1,2}/\\d{2}/\\d{4})",
                                       "%d/%m/%y",
                                       true // skip HTML stripping
                                       );
            result["Alkimia Currency"].setProfile(m_p);
            break;
        default:
            break;
        }
        return result;
    }
};

// define static members
QString AlkOnlineQuotesProfile::Private::m_financeQuoteScriptPath;
QStringList AlkOnlineQuotesProfile::Private::m_financeQuoteSources;


AlkOnlineQuotesProfile::AlkOnlineQuotesProfile(const QString &name, Type type,
                                               const QString &ghnsConfigFile)
    : d(new Private(this))
{
    d->m_name = name;
    d->m_GHNSFile = ghnsConfigFile;
    d->m_type = type;
    d->m_kconfigFile = name + "rc";
    if (type == Type::KMyMoney5)
        d->m_kconfigFile = QString("%1/.config/kmymoney/kmymoneyrc").arg(QDir::homePath());
    else if (type == Type::KMyMoney4)
        d->m_kconfigFile = QString("%1/.kde4/share/config/kmymoneyrc").arg(QDir::homePath());
    else if (type == Type::Alkimia5)
        d->m_kconfigFile = QString("%1/.config/kmymoney/alkimiarc").arg(QDir::homePath());
    else if (type == Type::Alkimia4)
        d->m_kconfigFile = QString("%1/.kde4/share/config/alkimiarc").arg(QDir::homePath());
    if (!d->m_kconfigFile.isEmpty())
        d->m_config = new KConfig(d->m_kconfigFile);
    if (!d->m_GHNSFile.isEmpty()) {
        KConfig ghnsFile(hotNewStuffConfigFile());
        KConfigGroup group = ghnsFile.group("KNewStuff3");
        d->m_GHNSFilePath = group.readEntry("TargetDir");
        d->checkUpdates();
    }
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
    if (configFile.isEmpty()) {
        configFile = QString("%1/%2").arg(KNSRC_DIR, d->m_GHNSFile);
    }

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

AlkOnlineQuotesProfile::Type AlkOnlineQuotesProfile::type()
{
    return d->m_type;
}

bool AlkOnlineQuotesProfile::hasGHNSSupport()
{
    return !d->m_GHNSFile.isEmpty();
}

const AlkOnlineQuotesProfile::Map AlkOnlineQuotesProfile::defaultQuoteSources()
{
    return d->defaultQuoteSources();
}

const QStringList AlkOnlineQuotesProfile::quoteSources()
{
    QStringList result;
    switch(d->m_type) {
    case AlkOnlineQuotesProfile::Type::Alkimia4:
    case AlkOnlineQuotesProfile::Type::Alkimia5:
    case AlkOnlineQuotesProfile::Type::KMyMoney4:
    case AlkOnlineQuotesProfile::Type::KMyMoney5:
        result << d->quoteSourcesNative();
        break;
    case AlkOnlineQuotesProfile::Type::Script:
        result << d->quoteSourcesFinanceQuote();
        break;
    case AlkOnlineQuotesProfile::Type::Skrooge:
        result << d->quoteSourcesSkrooge();
        break;
    default:
        break;
    }
    if (hasGHNSSupport())
        result << d->quoteSourcesGHNS();
    return result;
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
