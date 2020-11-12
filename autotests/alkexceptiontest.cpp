/***************************************************************************
 *   Copyright 2002  Thomas Baumgart  tbaumgart@kde.org                    *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#include "alkexceptiontest.h"

#include "test.h"

QTEST_GUILESS_MAIN(AlkExceptionTest)

void AlkExceptionTest::init()
{
}

void AlkExceptionTest::cleanup()
{
}

void AlkExceptionTest::testDefaultConstructor()
{
    const AlkException &e = ALKEXCEPTION("Message");
    QVERIFY(e.what() == "Message");
    QVERIFY(e.line() == __LINE__ - 2);
    QVERIFY(e.file() == __FILE__);
}

void AlkExceptionTest::testConstructor()
{
    AlkException e("New message", "Joe's file", 1234);
    QVERIFY(e.what() == "New message");
    QVERIFY(e.line() == 1234);
    QVERIFY(e.file() == "Joe's file");
}
