/*
 * SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker@freenet.de
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file is part of libalkimia.
 */

#include "alkonlinequotesmodel.h"

#include "alkonlinequotesprofile.h"
#include "alkonlinequotesource.h"

#include <klocalizedstring.h>

QString sourceTypeString(const AlkOnlineQuoteSource &source)
{
    if (source.isGHNS()) {
        if (source.profile()->GHNSFilePath(source.name()).isEmpty())
            return i18n("Remote unpublished");
        else
            return i18n("Remote");
    } else if (source.isFinanceQuote())
        return i18n("Finance::Quote");
    return i18n("Local");
}

AlkOnlineQuotesModel::AlkOnlineQuotesModel(AlkOnlineQuotesProfile *profile)
    : _profile(profile)
    , _sourceNames(_profile->quoteSources())
{
    connect(profile, SIGNAL(sourcesChanged()), this, SLOT(slotSourcesChanged()));
}

int AlkOnlineQuotesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 2;
}

int AlkOnlineQuotesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _sourceNames.size();
}

QVariant AlkOnlineQuotesModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if (index.row() < _sourceNames.size()) {
                switch(index.column()) {
                case Name:
                    return _sourceNames.at(index.row());
                case Source:
                    return sourceTypeString(AlkOnlineQuoteSource(_sourceNames.at(index.row()), _profile));
                }
            }
            break;
        case NameRole:
            return _sourceNames.at(index.row());
    }

    return QVariant();
}

Qt::ItemFlags AlkOnlineQuotesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled /*| QAbstractTableModel::flags(index)*/;
    switch (index.column()) {
        case Name: {
            QString name = data(index).toString();
            if (name.isEmpty())
                return flags;
            AlkOnlineQuoteSource source(name, _profile);
            if (source.isValid() && !source.isGHNS())
                flags |= Qt::ItemIsEditable;
            return flags;
        }
    }
    return flags;
}

QVariant AlkOnlineQuotesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case Name: return i18n("Name");
        case Source: return i18n("Source");
        }
    }

    return QVariant();
}

bool AlkOnlineQuotesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < _sourceNames.size()) {
        QMap<int, QString> sources;
        for (int i = row; i < row + count; i++) {
            QString name = _sourceNames.at(i);
            if (!_profile->defaultQuoteSources().contains(name))
                sources[i] = name;
        }
        for (const auto &i : sources.keys()) {
            AlkOnlineQuoteSource source(sources[i], _profile);
            if (source.isValid() && !source.isGHNS()) {
                beginRemoveRows(parent, i, i);
                source.remove();
                _sourceNames.removeAll(sources[i]);
                endRemoveRows();
            }
        }
        return true;
    }
    return false;
}

bool AlkOnlineQuotesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid()) {
        return false;
    }

    if (role == Qt::EditRole && index.row() < _sourceNames.size() && index.column() == Name) {
        AlkOnlineQuoteSource source(_sourceNames.at(index.row()).trimmed(), _profile);
        source.rename(value.toString());
        slotSourcesChanged();
        return true;
    }
    return false;
}

QModelIndex AlkOnlineQuotesModel::indexFromName(const QString &name)
{
    for (int i = 0; i < _sourceNames.size(); i++) {
        if (name == _sourceNames.at(i))
            return createIndex(i, 0, nullptr);
    }
    return createIndex(-1, -1, nullptr);
}

void AlkOnlineQuotesModel::slotSourcesChanged()
{
    int endRow = _sourceNames.size() - 1;
    _sourceNames = _profile->quoteSources();
    Q_EMIT dataChanged(createIndex(0, 0), createIndex(endRow, 1));
}
