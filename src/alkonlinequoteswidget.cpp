/***************************************************************************
 *   Copyright 2004  Thomas Baumgart  tbaumgart@kde.org                    *
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

#include "alkonlinequoteswidget.h"

#include "alkonlinequote.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"

#include <QRegExp>
#include <QCheckBox>
#include <QDesktopServices>
#include <QtDebug>
#include <QWebInspector>

#include <KConfig>
#include <KGlobal>
#include <KIcon>
#include <KIconLoader>
#include <KGuiItem>
#include <KMessageBox>
#include <KStandardDirs>
#include <knewstuff3/downloaddialog.h>
#include <knewstuff3/uploaddialog.h>

class AlkOnlineQuotesWidget::Private
{
public:
    QWebView *m_webView;
    QString m_acceptLanguage;
    QList<AlkOnlineQuoteSource> m_resetList;
    AlkOnlineQuoteSource m_currentItem;
    bool m_quoteInEditing;
    AlkOnlineQuotesProfile *m_profile;

    Private()
        : m_webView(nullptr)
        , m_quoteInEditing(false)
        , m_profile(nullptr)
    {
    }
};

AlkOnlineQuotesWidget::AlkOnlineQuotesWidget(QWidget *parent)
    : AlkOnlineQuotesWidgetDecl(parent)
    , d(new Private)
{
    loadProfiles();

    // TODO move to ui file
    KGuiItem updateButtenItem(i18nc("Accepts the entered data and stores it", "&Accept"),
                              KIcon("dialog-ok"),
                              i18n("Accepts the entered data and stores it"),
                              i18n("Use this to accept the modified data."));
    m_updateButton->setGuiItem(updateButtenItem);

    KGuiItem deleteButtenItem(i18n("&Delete"),
                              KIcon("edit-delete"),
                              i18n("Delete the selected source entry"),
                              i18n("Use this to delete the selected online source entry"));
    m_deleteButton->setGuiItem(deleteButtenItem);

    KGuiItem checkButtonItem(i18nc("Check the selected source entry", "&Check Source"),
                             KIcon("document-edit-verify"),
                             i18n("Check the selected source entry"),
                             i18n("Use this to check the selected online source entry"));
    m_checkButton->setGuiItem(checkButtonItem);

    KGuiItem showButtonItem(i18nc("Show the selected source entry in a web browser", "&Show page"),
                            KIcon("applications-internet"),
                            i18n("Show the selected source entry in a web browser"),
                            i18n("Use this to show the selected online source entry"));
    m_showButton->setGuiItem(showButtonItem);

    KGuiItem newButtenItem(i18nc("Create a new source entry for online quotes", "&New..."),
                           KIcon("document-new"),
                           i18n("Create a new source entry for online quotes"),
                           i18n("Use this to create a new entry for online quotes"));
    m_newButton->setGuiItem(newButtenItem);

    connect(m_newProfile, SIGNAL(clicked()), this, SLOT(slotNewProfile()));
    connect(m_deleteProfile, SIGNAL(clicked()), this, SLOT(slotDeleteProfile()));
    connect(m_selectProfile, SIGNAL(clicked()), this, SLOT(slotSelectProfile()));

    connect(m_updateButton, SIGNAL(clicked()), this, SLOT(slotUpdateEntry()));
    connect(m_newButton, SIGNAL(clicked()), this, SLOT(slotNewEntry()));
    connect(m_checkButton, SIGNAL(clicked()), this, SLOT(slotCheckEntry()));
    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
    connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstallEntries()));

    connect(m_quoteSourceList, SIGNAL(itemSelectionChanged()), this, SLOT(slotLoadWidgets()));
    connect(m_quoteSourceList, SIGNAL(itemChanged(QListWidgetItem *)), this,
            SLOT(slotEntryRenamed(QListWidgetItem *)));
    connect(m_quoteSourceList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this,
            SLOT(slotStartRename(QListWidgetItem *)));

    connect(m_editURL, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editSymbol, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editDate, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editDateFormat, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editPrice, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_skipStripping, SIGNAL(toggled(bool)), this, SLOT(slotEntryChanged()));

    m_checkSymbol->setText("ORCL");
    m_checkSymbol2->setText("BTC GBP");
    m_updateButton->setEnabled(false);
    m_showButton->setVisible(false);
    slotLoadProfile();
}

AlkOnlineQuotesWidget::~AlkOnlineQuotesWidget()
{
    delete d;
}

void AlkOnlineQuotesWidget::loadProfiles()
{
    AlkOnlineQuotesProfileList list = AlkOnlineQuotesProfileManager::instance().profiles();
    foreach (AlkOnlineQuotesProfile *profile, list) {
        QListWidgetItem *item = new QListWidgetItem(dynamic_cast<QListWidget *>(m_profileList));
        item->setText(profile->name());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
    m_profileList->setCurrentRow(0);
    d->m_profile = AlkOnlineQuotesProfileManager::instance().profiles().first();
    loadQuotesList();
}

void AlkOnlineQuotesWidget::loadQuotesList(const bool updateResetList)
{
    //disconnect the slot while items are being loaded and reconnect at the end
    disconnect(m_quoteSourceList, SIGNAL(itemChanged(QListWidgetItem *)), this,
               SLOT(slotEntryRenamed(QListWidgetItem *)));
    d->m_quoteInEditing = false;
    QStringList groups = d->m_profile->quoteSources();

    if (updateResetList) {
        d->m_resetList.clear();
    }
    m_quoteSourceList->clear();
    QStringList::Iterator it;
    for (it = groups.begin(); it != groups.end(); ++it) {
        AlkOnlineQuoteSource source(*it, d->m_profile);
        if (!source.isValid()) {
            continue;
        }

        QListWidgetItem *item = new QListWidgetItem(*it);
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_quoteSourceList->addItem(item);
        if (updateResetList) {
            d->m_resetList += source;
        }
    }
    m_quoteSourceList->sortItems();

    QListWidgetItem *first = m_quoteSourceList->item(0);
    if (first) {
        m_quoteSourceList->setCurrentItem(first);
    }
    slotLoadWidgets();

    m_newButton->setEnabled((m_quoteSourceList->findItems(i18n("New Quote Source"),
                                                          Qt::MatchExactly)).count() == 0);
    connect(m_quoteSourceList, SIGNAL(itemChanged(QListWidgetItem *)), this,
            SLOT(slotEntryRenamed(QListWidgetItem *)));
}

void AlkOnlineQuotesWidget::resetConfig()
{
    QStringList::ConstIterator it;
    QStringList groups = d->m_profile->quoteSources();

    // delete all currently defined entries
    for (it = groups.constBegin(); it != groups.constEnd(); ++it) {
        AlkOnlineQuoteSource(*it, d->m_profile).remove();
    }

    // and write back the one's from the reset list
    QList<AlkOnlineQuoteSource>::iterator itr;
    for (itr = d->m_resetList.begin(); itr != d->m_resetList.end(); ++itr) {
        (*itr).write();
    }

    loadQuotesList();
}

void AlkOnlineQuotesWidget::setView(QWebView *view)
{
    d->m_webView = view;
}

QString AlkOnlineQuotesWidget::acceptLanguage() const
{
    return d->m_acceptLanguage;
}

void AlkOnlineQuotesWidget::setAcceptLanguage(const QString &text)
{
    d->m_acceptLanguage = text;
}

void AlkOnlineQuotesWidget::slotNewProfile()
{
    QListWidgetItem *item = new QListWidgetItem(dynamic_cast<QListWidget *>(m_profileList));
    item->setText(QLatin1String("new profile"));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void AlkOnlineQuotesWidget::slotDeleteProfile()
{
    delete m_profileList->currentItem();
}

void AlkOnlineQuotesWidget::slotSelectProfile()
{
    slotLoadProfile();
}

void AlkOnlineQuotesWidget::slotLoadProfile()
{
    AlkOnlineQuotesProfileList list = AlkOnlineQuotesProfileManager::instance().profiles();
    foreach (AlkOnlineQuotesProfile *profile, list) {
        if (m_profileList->currentItem()->text() == profile->name()) {
            d->m_profile = profile;
            loadQuotesList();
            m_installButton->setVisible(profile->type() == AlkOnlineQuotesProfile::Type::GHNS);
        }
    }
}

void AlkOnlineQuotesWidget::slotLoadWidgets()
{
    d->m_quoteInEditing = false;
    QListWidgetItem *item = m_quoteSourceList->currentItem();

    m_editURL->setEnabled(true);
    m_editSymbol->setEnabled(true);
    m_editPrice->setEnabled(true);
    m_editDate->setEnabled(true);
    m_editDateFormat->setEnabled(true);
    m_skipStripping->setEnabled(true);
    m_editURL->setText(QString());
    m_editSymbol->setText(QString());
    m_editPrice->setText(QString());
    m_editDate->setText(QString());
    m_editDateFormat->setText(QString());

    if (item) {
        d->m_currentItem = AlkOnlineQuoteSource(item->text(), d->m_profile);
        m_editURL->setText(d->m_currentItem.url());
        m_editSymbol->setText(d->m_currentItem.sym());
        m_editPrice->setText(d->m_currentItem.price());
        m_editDate->setText(d->m_currentItem.date());
        m_editDateFormat->setText(d->m_currentItem.dateformat());
        m_skipStripping->setChecked(d->m_currentItem.skipStripping());
    } else {
        m_editURL->setEnabled(false);
        m_editSymbol->setEnabled(false);
        m_editPrice->setEnabled(false);
        m_editDate->setEnabled(false);
        m_editDateFormat->setEnabled(false);
        m_skipStripping->setEnabled(false);
    }

    m_updateButton->setEnabled(false);
}

void AlkOnlineQuotesWidget::slotEntryChanged()
{
    clearIcons();
    bool modified = m_editURL->text() != d->m_currentItem.url()
                    || m_editSymbol->text() != d->m_currentItem.sym()
                    || m_editDate->text() != d->m_currentItem.date()
                    || m_editDateFormat->text() != d->m_currentItem.dateformat()
                    || m_editPrice->text() != d->m_currentItem.price()
                    || m_skipStripping->isChecked() != d->m_currentItem.skipStripping();

    m_updateButton->setEnabled(modified);
    m_checkButton->setEnabled(!modified);
    m_checkSymbol->setEnabled(!d->m_currentItem.url().contains("%2"));
    m_checkSymbol2->setEnabled(d->m_currentItem.url().contains("%2"));
}

void AlkOnlineQuotesWidget::slotDeleteEntry()
{
    QList<QListWidgetItem *> items = m_quoteSourceList->findItems(
        d->m_currentItem.name(), Qt::MatchExactly);
    if (items.size() == 0) {
        return;
    }
    QListWidgetItem *item = items.at(0);
    if (!item) {
        return;
    }

    int ret = KMessageBox::warningContinueCancel(this,
                                                 i18n("Are you sure to delete this online quote ?"),
                                                 i18n("Delete online quote"),
                                                 KStandardGuiItem::cont(),
                                                 KStandardGuiItem::cancel(),
                                                 QString("DeletingOnlineQuote"));
    if (ret == KMessageBox::Cancel) {
        return;
    }

    // keep this order to avoid deleting the wrong current item
    d->m_currentItem.remove();
    delete item;
    slotEntryChanged();
}

void AlkOnlineQuotesWidget::slotUpdateEntry()
{
    d->m_currentItem.setUrl(m_editURL->text());
    d->m_currentItem.setSym(m_editSymbol->text());
    d->m_currentItem.setDate(m_editDate->text());
    d->m_currentItem.setDateformat(m_editDateFormat->text());
    d->m_currentItem.setPrice(m_editPrice->text());
    d->m_currentItem.setSkipStripping(m_skipStripping->isChecked());
    d->m_currentItem.write();
    m_checkButton->setEnabled(true);
    slotEntryChanged();
}

void AlkOnlineQuotesWidget::slotNewEntry()
{
    AlkOnlineQuoteSource newSource(i18n("New Quote Source"), d->m_profile);
    newSource.write();
    loadQuotesList();
    QListWidgetItem *item
        = m_quoteSourceList->findItems(i18n("New Quote Source"), Qt::MatchExactly).at(0);
    if (item) {
        m_quoteSourceList->setCurrentItem(item);
        slotLoadWidgets();
    }
}

void AlkOnlineQuotesWidget::clearIcons()
{
    QPixmap emptyIcon;
    m_urlCheckLabel->setPixmap(emptyIcon);
    m_dateCheckLabel->setPixmap(emptyIcon);
    m_priceCheckLabel->setPixmap(emptyIcon);
    m_symbolCheckLabel->setPixmap(emptyIcon);
    m_dateFormatCheckLabel->setPixmap(emptyIcon);
}

void AlkOnlineQuotesWidget::setupIcons(const AlkOnlineQuote::Errors &errors)
{
    QPixmap okIcon(BarIcon("dialog-ok-apply"));
    QPixmap failIcon(BarIcon("dialog-cancel"));

    if (errors & AlkOnlineQuote::Errors::URL) {
        m_urlCheckLabel->setPixmap(failIcon);
    } else {
        m_urlCheckLabel->setPixmap(okIcon);
        m_symbolCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::Symbol ? failIcon : okIcon);
        m_priceCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::Price ? failIcon : okIcon);
        if (errors & AlkOnlineQuote::Errors::Date) {
            m_dateCheckLabel->setPixmap(failIcon);
        } else {
            m_dateCheckLabel->setPixmap(okIcon);
            m_dateFormatCheckLabel->setPixmap(
                errors & AlkOnlineQuote::Errors::DateFormat ? failIcon : okIcon);
        }
    }
}

void AlkOnlineQuotesWidget::slotCheckEntry()
{
    AlkOnlineQuote quote(d->m_profile);
    m_logWindow->setVisible(true);
    m_logWindow->clear();
    clearIcons();
    quote.setWebView(d->m_webView);
    quote.setAcceptLanguage(d->m_acceptLanguage);

    connect(&quote, SIGNAL(status(QString)), this, SLOT(slotLogStatus(QString)));
    connect(&quote, SIGNAL(error(QString)), this, SLOT(slotLogError(QString)));
    connect(&quote, SIGNAL(failed(QString,QString)), this, SLOT(slotLogFailed(QString,QString)));
    connect(&quote, SIGNAL(quote(QString,QString,QDate,double)), this,
            SLOT(slotLogQuote(QString,QString,QDate,double)));
    if (d->m_currentItem.url().contains("%2")) {
        quote.launch(m_checkSymbol2->text(), m_checkSymbol2->text(), d->m_currentItem.name());
    } else {
        quote.launch(m_checkSymbol->text(), m_checkSymbol->text(), d->m_currentItem.name());
    }
    setupIcons(quote.errors());
}

void AlkOnlineQuotesWidget::slotLogStatus(const QString &s)
{
    m_logWindow->append(s);
}

void AlkOnlineQuotesWidget::slotLogError(const QString &s)
{
    slotLogStatus(QString("<font color=\"red\"><b>") + s + QString("</b></font>"));
}

void AlkOnlineQuotesWidget::slotLogFailed(const QString &id, const QString &symbol)
{
    slotLogStatus(QString("%1 %2").arg(id, symbol));
}

void AlkOnlineQuotesWidget::slotLogQuote(const QString &id, const QString &symbol,
                                         const QDate &date, double price)
{
    slotLogStatus(QString("<font color=\"green\">%1 %2 %3 %4</font>").arg(id, symbol,
                                                                          date.toString()).arg(
                      price));
}

void AlkOnlineQuotesWidget::slotStartRename(QListWidgetItem *item)
{
    d->m_quoteInEditing = true;
    m_quoteSourceList->editItem(item);
}

void AlkOnlineQuotesWidget::slotEntryRenamed(QListWidgetItem *item)
{
    //if there is no current item selected, exit
    if (d->m_quoteInEditing == false || !m_quoteSourceList->currentItem()
        || item != m_quoteSourceList->currentItem()) {
        return;
    }

    d->m_quoteInEditing = false;
    QString text = item->text();
    int nameCount = 0;
    for (int i = 0; i < m_quoteSourceList->count(); ++i) {
        if (m_quoteSourceList->item(i)->text() == text) {
            ++nameCount;
        }
    }

    // Make sure we get a non-empty and unique name
    if (text.length() > 0 && nameCount == 1) {
        d->m_currentItem.rename(text);
    } else {
        item->setText(d->m_currentItem.name());
    }
    m_quoteSourceList->sortItems();
    m_newButton->setEnabled(m_quoteSourceList->findItems(i18n(
                                                             "New Quote Source"),
                                                         Qt::MatchExactly).count() == 0);
}

void AlkOnlineQuotesWidget::slotInstallEntries()
{
    QString configFile = d->m_profile->hotNewStuffConfigFile();

    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog(configFile, this);
    dialog->exec();
    delete dialog;
    loadQuotesList();
}

QString AlkOnlineQuotesWidget::expandedUrl() const
{
    if (d->m_currentItem.url().contains("%2")) {
        return d->m_currentItem.url().arg(m_checkSymbol2->text());
    } else {
        return d->m_currentItem.url().arg(m_checkSymbol->text());
    }
}