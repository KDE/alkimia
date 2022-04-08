/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/
#ifndef QMLALKIMIAPLUGIN_H
#define QMLALKIMIAPLUGIN_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QtQml>
#else
#include <QtDeclarative>
class QQmlExtensionPlugin : public QDeclarativeExtensionPlugin
{
};
#undef Q_PLUGIN_METADATA
#define Q_PLUGIN_METADATA(s)
#define Q_DECL_OVERRIDE
#endif

class QmlAlkimiaPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.alkimia")

public:
    void registerTypes(const char *uri) Q_DECL_OVERRIDE;
};

#endif // QMLALKIMIAPLUGIN_H
