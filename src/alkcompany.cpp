/*
    SPDX-FileCopyrightText: 2011 Alvaro Soliverez asoliverez @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkcompany.h"

#include <QDBusMetaType>
#include <QDBusArgument>

class AlkCompany::Private
{
public:
    QString m_symbol;
    QString m_name;
    QString m_type;
    QString m_exchange;
    QString m_id;
};

AlkCompany::AlkCompany(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

AlkCompany::~AlkCompany()
{
}

AlkCompany::AlkCompany(const AlkCompany &company, QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    setName(company.name());
    setSymbol(company.symbol());
    setType(company.type());
    setExchange(company.exchange());
    setRecordId(company.recordId());
}

const QString &AlkCompany::symbol() const
{
    return d->m_symbol;
}

const QString &AlkCompany::name() const
{
    return d->m_name;
}

const QString &AlkCompany::type() const
{
    return d->m_type;
}

const QString &AlkCompany::exchange() const
{
    return d->m_exchange;
}

const QString &AlkCompany::recordId() const
{
    return d->m_id;
}

void AlkCompany::setSymbol(const QString &symbol)
{
    d->m_symbol = symbol;
}

void AlkCompany::setName(const QString &name)
{
    d->m_name = name;
}

void AlkCompany::setType(const QString &type)
{
    d->m_type = type;
}

void AlkCompany::setExchange(const QString &exchange)
{
    d->m_exchange = exchange;
}

void AlkCompany::setRecordId(const QString &recordId)
{
    d->m_id = recordId;
}

QDBusArgument &operator<<(QDBusArgument &argument, const AlkCompany &company)
{
    argument.beginStructure();
    argument << company.symbol() << company.name() << company.type() << company.exchange()
             << company.recordId();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, AlkCompany &company)
{
    argument.beginStructure();
    QString symbol;
    QString name;
    QString type;
    QString exchange;
    QString id;
    argument >> symbol >> name >> type >> exchange >> id;
    company.setSymbol(symbol);
    company.setName(name);
    company.setType(type);
    company.setExchange(exchange);
    company.setRecordId(id);
    argument.endStructure();
    return argument;
}
