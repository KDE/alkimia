/*
    SPDX-FileCopyrightText: 2020 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKPUSHBUTTON_H
#define ALKPUSHBUTTON_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QPushButton>
#define AlkPushButton QPushButton
#else
#include <KPushButton>
#define AlkPushButton KPushButton
#endif

#endif
