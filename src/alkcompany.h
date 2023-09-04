/*
    SPDX-FileCopyrightText: 2011 Alvaro Soliverez asoliverez @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKCOMPANY_H
#define ALKCOMPANY_H

#include <alkimia/alk_export.h>

#include <QMetaType>
#include <QDBusArgument>
#include <QObject>
#include <QString>

class ALK_EXPORT AlkCompany : public QObject
{
    Q_OBJECT
public:
    explicit AlkCompany(QObject *parent = 0);
    AlkCompany(const AlkCompany &company, QObject *parent = 0);
    ~AlkCompany();

    /**
     * Financial symbol
     */
    const QString &symbol() const;

    /**
     * Name of the equity
     */
    const QString &name() const;

    /**
     * Type of the equity
     */
    const QString &type() const;

    /**
     * Exchange market of the equity
     */
    const QString &exchange() const;

    /**
     * Internal id
     */
    const QString &recordId() const;

    void setSymbol(const QString &symbol);
    void setName(const QString &name);
    void setType(const QString &type);
    void setExchange(const QString &exchange);
    void setRecordId(const QString &recordId);

private:
    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private *const d;
};

ALK_EXPORT QDBusArgument &operator<<(QDBusArgument &argument, const AlkCompany &company);
ALK_EXPORT const QDBusArgument &operator>>(const QDBusArgument &argument, AlkCompany &company);

Q_DECLARE_METATYPE(AlkCompany)

#endif
