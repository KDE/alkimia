/*
    SPDX-FileCopyrightText: 2010 Thomas Baumgart ipwizard @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKVALUETEST_H
#define ALKVALUETEST_H

#include <QtCore/QObject>

class AlkValue;

class AlkValueTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void emptyCtor();
    void copyCtor();
    void intCtor();
    void stringCtor();
    void doubleCtor();
    void assignment();
    void equality();
    void inequality();
    void less();
    void greater();
    void lessThan();
    void greaterThan();
    void addition();
    void subtraction();
    void multiplication();
    void division();
    void modulo();
    void unaryMinus();
    void abs();
    void precision();
    void convertDenominator();
    void convertPrecision();
    void denominatorToPrecision();
    void precisionToDenominator();
    void valueRef();
    void canonicalize();
};

#endif // ALKVALUETEST_H
