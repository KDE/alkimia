/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkdebug.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(alkimia, "Alkimia")
QDebug _alkDebug(const char *file, int line, const char *func)
{
    return QMessageLogger(file, line, func, alkimia().categoryName()).debug() << func;
}
#else
#include <KDebug>

static int dbgArea()
{
    static int s_area = KDebug::registerArea("Alkimia");
    return s_area;
}

QDebug _alkDebug(const char *file, int line, const char *func)
{
    return kDebugStream(QtDebugMsg, dbgArea(), file, line, func);
}
#endif
