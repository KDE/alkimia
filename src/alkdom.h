/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKDOM_H
#define ALKDOM_H

#include <alkimia/alk_export.h>

#include <QString>
#include <QStringList>

class AlkDomDocument;

/**
 * The class AlkDomElement is a replacement for QDomElement
 * with the possibility to store attributes sorted.
 *
 * Sorted attributes are important for a textual comparison.
 *
 * @author Ralf Habacker <ralf.habacker@freenet.de>
 */
class ALK_EXPORT AlkDomElement
{
public:
    explicit AlkDomElement(const QString &tag = QString());
    virtual ~AlkDomElement();

    void setComment(const QString &comment);
    void setAttribute(const QString &name, const QString &value);
    void setAttribute(const QString &name, double value);
    void appendChild(const AlkDomElement &element);
    virtual QString toString(bool withIndentation = true, int level = 0) const;

protected:
    QString m_tag;
    QString m_comment;
    QStringList m_attributes;
    QList<AlkDomElement> m_childs;
};

/**
 * The class AlkDomDocument is a simple replacement for QDomDocument.
 *
 * @author Ralf Habacker <ralf.habacker@freenet.de>
 */
class ALK_EXPORT AlkDomDocument : public AlkDomElement
{
public:
    AlkDomDocument(const QString &type = QString());
    virtual ~AlkDomDocument() override;

    AlkDomElement createElement(const QString &name);
    QString toString(bool withIndentation = true, int level = 0) const override;

protected:
    QString m_type;
};

#endif // ALKDOM_H
