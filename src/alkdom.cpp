/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdom.h"

AlkDomElement::AlkDomElement(const QString &tag)
    : m_tag(tag)
{
}

AlkDomElement::~AlkDomElement()
{
}

void AlkDomElement::setAttribute(const QString &name, const QString &value)
{
    m_attributes.append(QString("%1=\"%2\"").arg(name, value));
}

void AlkDomElement::setAttribute(const QString &name, double value)
{
    m_attributes.append(QString("%1=\"%2\"").arg(name).arg(value));
}

void AlkDomElement::appendChild(const AlkDomElement &element)
{
    m_childs.append(element);
}

QString AlkDomElement::toString(bool withIndentation, int level) const
{
    QString prefix = withIndentation ? QString(level, ' ') : QString();
    QStringList attributes(m_attributes);
    attributes.sort();
    if (m_childs.size() > 0) {
        QString s = !m_tag.isEmpty()
            ? attributes.size() > 0 ? QString("%1<%2 %3>\n").arg(prefix, m_tag, attributes.join(" ")) : QString("%1<%2>\n").arg(prefix, m_tag)
            : "";
        for (const AlkDomElement &child : m_childs) {
            s += child.toString(withIndentation, level + 1);
        }
        s += !m_tag.isEmpty() ? QString("%1</%2>\n").arg(prefix, m_tag) : "";
        return s;
    } else {
        QString s = !m_tag.isEmpty()
            ? attributes.size() > 0 ? QString("%1<%2 %3 />\n").arg(prefix, m_tag, attributes.join(" ")) : QString("%1<%2 />\n").arg(prefix, m_tag)
            : "";
        return s;
    }
}

AlkDomDocument::AlkDomDocument(const QString &type)
    : AlkDomElement("")
    , m_type(type)
{
}

AlkDomDocument::~AlkDomDocument()
{
}

AlkDomElement AlkDomDocument::createElement(const QString &name)
{
    return AlkDomElement(name);
}

QString AlkDomDocument::toString(bool withIndentation, int level) const
{
    QString s = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    if (!m_type.isEmpty())
        s.append(QString("<!DOCTYPE %1>\n").arg(m_type));
    s.append(AlkDomElement::toString(withIndentation, level));
    return s;
}
