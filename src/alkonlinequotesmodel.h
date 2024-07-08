/*
 * SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker@freenet.de
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of libalkimia.
 */

#ifndef ALKONLINEQUOTESMODEL_H
#define ALKONLINEQUOTESMODEL_H

#include <QAbstractItemModel>
#include <QStringList>

class AlkOnlineQuotesProfile;

class AlkOnlineQuotesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns {
        Name,
        Source,
    };

    enum Roles {
        NameRole = Qt::UserRole
    };

    explicit AlkOnlineQuotesModel(AlkOnlineQuotesProfile *profile);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void setProfile(AlkOnlineQuotesProfile* profile);

public Q_SLOTS:
    void slotSourcesChanged();

protected:
    AlkOnlineQuotesProfile *_profile;
    QStringList _sourceNames;
};

#endif // ALKONLINEQUOTESMODEL_H
