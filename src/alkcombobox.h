/*
    SPDX-FileCopyrightText: 2020 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKCOMBOBOX_H
#define ALKCOMBOBOX_H

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <KComboBox>
#define AlkComboBox KComboBox
#else
#include "kcombobox.h"
class AlkComboBox : public KComboBox
{
public:
    AlkComboBox(QWidget *parent)
        : KComboBox(parent)
    {
    }

    QVariant currentData()
    {
        return itemData(currentIndex());
    }
};
#endif

#endif
