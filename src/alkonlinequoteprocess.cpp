/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
