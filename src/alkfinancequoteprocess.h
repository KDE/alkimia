/*
    SPDX-FileCopyrightText: 2004 Ace Jones acejones @users.sourceforge.net
    SPDX-FileCopyrightText: 2004 Thomas Baumgart <thb@net-bembel.de>

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKFINANCEQUOTEPROCESS_H
#define ALKFINANCEQUOTEPROCESS_H

#include <alkimia/alk_export.h>

#include <KProcess>

/**
Helper class to run the Finance::Quote process. This is used only for the purpose of obtaining
a list of valid sources. The actual price quotes are obtained thru AlkFinanceQouteProcess.
The class also contains functions to convert between the rather cryptic source names used
by the Finance::Quote package, and more user-friendly names.

@author Thomas Baumgart <thb@net-bembel.de> & Ace Jones acejones @users.sourceforge.net, Tony B<tonybloom@users.sourceforge.net>
 */
class ALK_EXPORT AlkFinanceQuoteProcess : public KProcess
{
    Q_OBJECT
public:
    AlkFinanceQuoteProcess();
    void launch(const QString &scriptPath);
    bool isFinished() const;
    const QStringList getSourceList() const;
    const QString crypticName(const QString &niceName) const;
    const QString niceName(const QString &crypticName) const;

public slots:
    void slotReceivedDataFromFilter();
    void slotProcessExited();

private:
    class Private;
    Private *const d;
};

#endif // ALKFINANCEQUOTEPROCESS_H
