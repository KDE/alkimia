/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2004 Thomas Baumgart <thb@net-bembel.de>

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKONLINEQUOTEPROCESS_H
#define ALKONLINEQUOTEPROCESS_H

#include <alkimia/alk_export.h>

#include <KProcess>

/**
Helper class to attend the process which is running the script, in the case
of a local script being used to fetch the quote.

@author Thomas Baumgart <thb@net-bembel.de> & Ace Jones acejones @users.sourceforge.net
*/
class ALK_NO_EXPORT AlkOnlineQuoteProcess : public KProcess
{
    Q_OBJECT
public:
    AlkOnlineQuoteProcess();
    inline void setSymbol(const QString &_symbol)
    {
        m_symbol = _symbol;
        m_string.truncate(0);
    }

public Q_SLOTS:
    void slotReceivedDataFromFilter();
    void slotProcessExited(int exitCode, QProcess::ExitStatus exitStatus);

Q_SIGNALS:
    void processExited(const QString &);

private:
    QString m_symbol;
    QString m_string;
};

#endif // ALKONLINEQUOTEPROCESS_H
