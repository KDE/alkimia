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

#ifndef ALKONLINEQUOTESWIDGET_H
#define ALKONLINEQUOTESWIDGET_H

#include "alk_export.h"

#include <alkonlinequote.h>

#include <QList>

#include <ui_alkonlinequoteswidget.h>

class ALK_NO_EXPORT AlkOnlineQuotesWidgetDecl : public QWidget, public Ui::AlkOnlineQuotesWidget
{
public:
  AlkOnlineQuotesWidgetDecl(QWidget *parent) : QWidget(parent) {
    setupUi(this);
  }
};

class ALK_EXPORT AlkOnlineQuotesWidget : public AlkOnlineQuotesWidgetDecl
{
  Q_OBJECT
public:
  AlkOnlineQuotesWidget(QWidget* parent = 0);
  virtual ~AlkOnlineQuotesWidget() {}

  void writeConfig() {}
  void readConfig() {}
  void resetConfig();

protected slots:
  void slotDeleteEntry();
  void slotShowEntry();
  void slotUpdateEntry();
  void slotLoadWidgets();
  void slotEntryChanged();
  void slotNewEntry();
  void slotCheckEntry();
  void slotLogStatus(const QString &s);
  void slotLogFailed(const QString &id, const QString &symbol);
  void slotLogQuote(const QString &id, const QString &symbol, const QDate &date, double price);
  void slotEntryRenamed(QListWidgetItem* item);
  void slotStartRename(QListWidgetItem* item);
  void slotInstallEntries();

protected:
  void loadList(const bool updateResetList = false);
  void clearIcons();
  void setupIcons(const AlkOnlineQuote::Errors &errors);
  QString singleSymbol() const;
  QStringList doubleSymbol() const;
  QString expandedUrl() const;

private:
  QList<AlkOnlineQuoteSource>  m_resetList;
  AlkOnlineQuoteSource         m_currentItem;
  bool                           m_quoteInEditing;
};

#endif
