/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "qmlalkimiaplugin.h"

#include "alkonlinequote.h"

void QmlAlkimiaPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.alkimia"));
    qmlRegisterType<AlkOnlineQuote>(uri, 1, 0, "AlkOnlineQuote");
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(qmlalkimiaplugin, QmlAlkimiaPlugin);
#endif
