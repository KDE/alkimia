/***************************************************************************
 *   Copyright 2004  Thomas Baumgart  tbaumgart@kde.org                    *
 *   Copyright 2018  Ralf Habacker <ralf.habacker@freenet.de>              *
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

#include <alkimia/alkonlinequote.h>

#include <QWidget>

class QListWidgetItem;

class ALK_EXPORT AlkOnlineQuotesWidget : public QWidget
{
    Q_OBJECT
public:
    AlkOnlineQuotesWidget(bool showProfiles = false, bool showUpload = false, QWidget *parent = 0);
    virtual ~AlkOnlineQuotesWidget();

    void writeConfig();
    void readConfig();
    void resetConfig();

    QWidget *profilesWidget();
    QWidget *profileDetailsWidget();
    QWidget *onlineQuotesWidget();
    QWidget *quoteDetailsWidget();
    QWidget *debugWidget();

    QString acceptLanguage() const;
    void setAcceptLanguage(const QString &text);

private:
    class Private;
    Private *const d;
};

#endif
