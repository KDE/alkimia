/*
    SPDX-FileCopyrightText: 2020 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKCOMBOBOX_H
#define ALKCOMBOBOX_H

#include <QtGlobal>

#include "kcombobox.h"
class AlkComboBox : public KComboBox
{
public:
    AlkComboBox(QWidget *parent)
        : KComboBox(parent)
    {
    }

    template<class T> void setData(T value)
    {
        setCurrentIndex(findData(value));
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QVariant currentData()
    {
        return itemData(currentIndex());
    }
#endif
};

#endif
