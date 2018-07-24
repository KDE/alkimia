/***************************************************************************
 *   Copyright 2004  Ace Jones <acejones@users.sourceforge.net>            *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#include "alkonlinequoteprocess.h"

//
// Helper class to babysit the KProcess used for running the local script in that case
//

AlkOnlineQuoteProcess::AlkOnlineQuoteProcess()
{
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReceivedDataFromFilter()));
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), this,
            SLOT(slotProcessExited(int,QProcess::ExitStatus)));
}

void AlkOnlineQuoteProcess::slotReceivedDataFromFilter()
{
//   kDebug(2) << "WebPriceQuoteProcess::slotReceivedDataFromFilter(): " << QString(data);
    m_string += QString(readAllStandardOutput());
}

void AlkOnlineQuoteProcess::slotProcessExited(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
//   kDebug(2) << "WebPriceQuoteProcess::slotProcessExited()";
    emit processExited(m_string);
    m_string.truncate(0);
}
