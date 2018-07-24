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

#include <QRegExp>
#include <QCheckBox>
#include <QDesktopServices>

#include <KConfig>
#include <KGlobal>
#include <KIconLoader>
#include <KGuiItem>
#include <KMessageBox>
#include <KStandardDirs>
#include <knewstuff3/downloaddialog.h>
#include <knewstuff3/uploaddialog.h>

AlkOnlineQuotesWidget::AlkOnlineQuotesWidget(QWidget *parent)
 : AlkOnlineQuotesWidgetDecl(parent),
   m_quoteInEditing(false)
{
  QStringList groups = AlkOnlineQuote::quoteSources();

  loadList(true /*updateResetList*/);

  m_updateButton->setEnabled(false);

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

  connect(m_updateButton, SIGNAL(clicked()), this, SLOT(slotUpdateEntry()));
  connect(m_newButton, SIGNAL(clicked()), this, SLOT(slotNewEntry()));
  connect(m_checkButton, SIGNAL(clicked()), this, SLOT(slotCheckEntry()));
  connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
  connect(m_showButton, SIGNAL(clicked()), this, SLOT(slotShowEntry()));
  connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstallEntries()));

  connect(m_quoteSourceList, SIGNAL(itemSelectionChanged()), this, SLOT(slotLoadWidgets()));
  connect(m_quoteSourceList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotEntryRenamed(QListWidgetItem*)));
  connect(m_quoteSourceList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotStartRename(QListWidgetItem*)));

  connect(m_editURL, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
  connect(m_editSymbol, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
  connect(m_editDate, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
  connect(m_editDateFormat, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
  connect(m_editPrice, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
  connect(m_skipStripping, SIGNAL(toggled(bool)), this, SLOT(slotEntryChanged()));

  m_checkSymbol->setText("ORCL");
  m_checkSymbol2->setText("BTC GBP");
}

void AlkOnlineQuotesWidget::loadList(const bool updateResetList)
{
  //disconnect the slot while items are being loaded and reconnect at the end
  disconnect(m_quoteSourceList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotEntryRenamed(QListWidgetItem*)));
  m_quoteInEditing = false;
  QStringList groups = AlkOnlineQuote::quoteSources();

  if (updateResetList)
    m_resetList.clear();
  m_quoteSourceList->clear();
  QStringList::Iterator it;
  for (it = groups.begin(); it != groups.end(); ++it) {
    AlkOnlineQuoteSource source(*it);
    if (!source.isValid())
      continue;

    QListWidgetItem* item = new QListWidgetItem(*it);
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_quoteSourceList->addItem(item);
    if (updateResetList)
      m_resetList += source;
  }
  m_quoteSourceList->sortItems();

  QListWidgetItem* first = m_quoteSourceList->item(0);
  if (first)
    m_quoteSourceList->setCurrentItem(first);
  slotLoadWidgets();

  m_newButton->setEnabled((m_quoteSourceList->findItems(i18n("New Quote Source"), Qt::MatchExactly)).count() == 0);
  connect(m_quoteSourceList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(slotEntryRenamed(QListWidgetItem*)));
}

void AlkOnlineQuotesWidget::resetConfig()
{
  QStringList::ConstIterator it;
  QStringList groups = AlkOnlineQuote::quoteSources();

  // delete all currently defined entries
  for (it = groups.constBegin(); it != groups.constEnd(); ++it) {
    AlkOnlineQuoteSource(*it).remove();
  }

  // and write back the one's from the reset list
  QList<AlkOnlineQuoteSource>::ConstIterator itr;
  for (itr = m_resetList.constBegin(); itr != m_resetList.constEnd(); ++itr) {
    (*itr).write();
  }

  loadList();
}

void AlkOnlineQuotesWidget::slotLoadWidgets()
{
  m_quoteInEditing = false;
  QListWidgetItem* item = m_quoteSourceList->currentItem();

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
    m_currentItem = AlkOnlineQuoteSource(item->text());
    m_editURL->setText(m_currentItem.m_url);
    m_editSymbol->setText(m_currentItem.m_sym);
    m_editPrice->setText(m_currentItem.m_price);
    m_editDate->setText(m_currentItem.m_date);
    m_editDateFormat->setText(m_currentItem.m_dateformat);
    m_skipStripping->setChecked(m_currentItem.m_skipStripping);
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
  bool modified = m_editURL->text() != m_currentItem.m_url
                  || m_editSymbol->text() != m_currentItem.m_sym
                  || m_editDate->text() != m_currentItem.m_date
                  || m_editDateFormat->text() != m_currentItem.m_dateformat
                  || m_editPrice->text() != m_currentItem.m_price
                  || m_skipStripping->isChecked() != m_currentItem.m_skipStripping;

  m_updateButton->setEnabled(modified);
  m_checkButton->setEnabled(!modified);
  m_checkSymbol->setEnabled(!m_currentItem.m_url.contains("%2"));
  m_checkSymbol2->setEnabled(m_currentItem.m_url.contains("%2"));
}

void AlkOnlineQuotesWidget::slotDeleteEntry()
{
  QList<QListWidgetItem*> items = m_quoteSourceList->findItems(m_currentItem.m_name, Qt::MatchExactly);
  if (items.size() == 0)
    return;
  QListWidgetItem* item = items.at(0);
  if (!item)
    return;

  int ret = KMessageBox::warningContinueCancel(this,
                i18n("Are you sure to delete this online quote ?"),
                i18n("Delete online quote"),
                KStandardGuiItem::cont(),
                KStandardGuiItem::cancel(),
                QString("DeletingOnlineQuote"));
  if (ret == KMessageBox::Cancel)
    return;

  // keep this order to avoid deleting the wrong current item
  m_currentItem.remove();
  delete item;
  slotEntryChanged();
}

void AlkOnlineQuotesWidget::slotShowEntry()
{
  QDesktopServices::openUrl(expandedUrl());
}

void AlkOnlineQuotesWidget::slotUpdateEntry()
{
  m_currentItem.m_url = m_editURL->text();
  m_currentItem.m_sym = m_editSymbol->text();
  m_currentItem.m_date = m_editDate->text();
  m_currentItem.m_dateformat = m_editDateFormat->text();
  m_currentItem.m_price = m_editPrice->text();
  m_currentItem.m_skipStripping = m_skipStripping->isChecked();
  m_currentItem.write();
  m_checkButton->setEnabled(true);
  slotEntryChanged();
}

void AlkOnlineQuotesWidget::slotNewEntry()
{
  AlkOnlineQuoteSource newSource(i18n("New Quote Source"));
  newSource.write();
  loadList();
  QListWidgetItem* item = m_quoteSourceList->findItems(i18n("New Quote Source"), Qt::MatchExactly).at(0);
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

  if (errors & AlkOnlineQuote::Errors::URL)
    m_urlCheckLabel->setPixmap(failIcon);
  else {
    m_urlCheckLabel->setPixmap(okIcon);
    m_symbolCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::Symbol ? failIcon : okIcon);
    m_priceCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::Price ? failIcon : okIcon);
    if (errors & AlkOnlineQuote::Errors::Date)
      m_dateCheckLabel->setPixmap(failIcon);
    else {
      m_dateCheckLabel->setPixmap(okIcon);
      m_dateFormatCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::DateFormat? failIcon : okIcon);
    }
  }
}

void AlkOnlineQuotesWidget::slotCheckEntry()
{
  AlkOnlineQuote quote;
  m_logWindow->setVisible(true);
  m_logWindow->clear();
  clearIcons();

  connect(&quote, SIGNAL(status(QString)), this, SLOT(slotLogStatus(QString)));
  connect(&quote, SIGNAL(error(QString)), this, SLOT(slotLogStatus(QString)));
  connect(&quote, SIGNAL(failed(QString, QString)), this, SLOT(slotLogFailed(QString, QString)));
  connect(&quote, SIGNAL(quote(QString, QString, QDate, double)), this, SLOT(slotLogQuote(QString, QString, QDate, double)));
  if (m_currentItem.m_url.contains("%2"))
    quote.launch(m_checkSymbol2->text(), m_checkSymbol2->text(), m_currentItem.m_name);
  else
    quote.launch(m_checkSymbol->text(), m_checkSymbol->text(), m_currentItem.m_name);
  setupIcons(quote.errors());
}

void AlkOnlineQuotesWidget::slotLogStatus(const QString &s)
{
    new QListWidgetItem(s, m_logWindow);
    m_logWindow->scrollToBottom();
}

void AlkOnlineQuotesWidget::slotLogFailed(const QString &id, const QString &symbol)
{
    new QListWidgetItem(QString("%1 %2").arg(id, symbol), m_logWindow);
    m_logWindow->scrollToBottom();
}

void AlkOnlineQuotesWidget::slotLogQuote(const QString &id, const QString &symbol, const QDate &date, double price)
{
  slotLogStatus(QString("%1 %2 %3 %4").arg(id, symbol, date.toString()).arg(price));
}

void AlkOnlineQuotesWidget::slotStartRename(QListWidgetItem* item)
{
  m_quoteInEditing = true;
  m_quoteSourceList->editItem(item);
}

void AlkOnlineQuotesWidget::slotEntryRenamed(QListWidgetItem* item)
{
  //if there is no current item selected, exit
  if (m_quoteInEditing == false || !m_quoteSourceList->currentItem() || item != m_quoteSourceList->currentItem())
    return;

  m_quoteInEditing = false;
  QString text = item->text();
  int nameCount = 0;
  for (int i = 0; i < m_quoteSourceList->count(); ++i) {
    if (m_quoteSourceList->item(i)->text() == text)
      ++nameCount;
  }

  // Make sure we get a non-empty and unique name
  if (text.length() > 0 && nameCount == 1) {
    m_currentItem.rename(text);
  } else {
    item->setText(m_currentItem.m_name);
  }
  m_quoteSourceList->sortItems();
  m_newButton->setEnabled(m_quoteSourceList->findItems(i18n("New Quote Source"), Qt::MatchExactly).count() == 0);
}

void AlkOnlineQuotesWidget::slotInstallEntries()
{
  QString configFile = "skrooge_unit.knsrc";
  if (KStandardDirs::locate("config", configFile).isEmpty()) {
    configFile.prepend(QString("%1/").arg(KNSRC_DIR));
  }

  QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog(configFile, this);
  dialog->exec();
  delete dialog;
  loadList();
}

QString AlkOnlineQuotesWidget::expandedUrl() const
{
  if (m_currentItem.m_url.contains("%2"))
    return m_currentItem.m_url.arg(m_checkSymbol2->text());
  else
    return m_currentItem.m_url.arg(m_checkSymbol->text());
}
