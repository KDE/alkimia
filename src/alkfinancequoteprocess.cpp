/***************************************************************************
 *   Copyright 2004  Ace Jones <acejones@users.sourceforge.net>            *
 *   Copyright 2004  Thomas Baumgart <thb@net-bembel.de>                   *
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

#include "alkfinancequoteprocess.h"

#include <QString>
#include <QMap>

class AlkFinanceQuoteProcess::Private
{
public:
    typedef QMap<QString, QString> fqNameMap;
    bool m_isDone;
    QString m_string;
    fqNameMap m_fqNames;

    Private() : m_isDone(false)
    {
    }
};

AlkFinanceQuoteProcess::AlkFinanceQuoteProcess()
  : d(new Private)
{
    d->m_fqNames["aex"] = "AEX";
    d->m_fqNames["aex_futures"] = "AEX Futures";
    d->m_fqNames["aex_options"] = "AEX Options";
    d->m_fqNames["amfiindia"] = "AMFI India";
    d->m_fqNames["asegr"] = "ASE";
    d->m_fqNames["asia"] = "Asia (Yahoo, ...)";
    d->m_fqNames["asx"] = "ASX";
    d->m_fqNames["australia"] = "Australia (ASX, Yahoo, ...)";
    d->m_fqNames["bmonesbittburns"] = "BMO NesbittBurns";
    d->m_fqNames["brasil"] = "Brasil (Yahoo, ...)";
    d->m_fqNames["canada"] = "Canada (Yahoo, ...)";
    d->m_fqNames["canadamutual"] = "Canada Mutual (Fund Library, ...)";
    d->m_fqNames["deka"] = "Deka Investments";
    d->m_fqNames["dutch"] = "Dutch (AEX, ...)";
    d->m_fqNames["dwsfunds"] = "DWS";
    d->m_fqNames["europe"] = "Europe (Yahoo, ...)";
    d->m_fqNames["fidelity"] = "Fidelity (Fidelity, ...)";
    d->m_fqNames["fidelity_direct"] = "Fidelity Direct";
    d->m_fqNames["financecanada"] = "Finance Canada";
    d->m_fqNames["ftportfolios"] = "First Trust (First Trust, ...)";
    d->m_fqNames["ftportfolios_direct"] = "First Trust Portfolios";
    d->m_fqNames["fundlibrary"] = "Fund Library";
    d->m_fqNames["greece"] = "Greece (ASE, ...)";
    d->m_fqNames["indiamutual"] = "India Mutual (AMFI, ...)";
    d->m_fqNames["maninv"] = "Man Investments";
    d->m_fqNames["fool"] = "Motley Fool";
    d->m_fqNames["nasdaq"] = "Nasdaq (Yahoo, ...)";
    d->m_fqNames["nz"] = "New Zealand (Yahoo, ...)";
    d->m_fqNames["nyse"] = "NYSE (Yahoo, ...)";
    d->m_fqNames["nzx"] = "NZX";
    d->m_fqNames["platinum"] = "Platinum Asset Management";
    d->m_fqNames["seb_funds"] = "SEB";
    d->m_fqNames["sharenet"] = "Sharenet";
    d->m_fqNames["za"] = "South Africa (Sharenet, ...)";
    d->m_fqNames["troweprice_direct"] = "T. Rowe Price";
    d->m_fqNames["troweprice"] = "T. Rowe Price";
    d->m_fqNames["tdefunds"] = "TD Efunds";
    d->m_fqNames["tdwaterhouse"] = "TD Waterhouse Canada";
    d->m_fqNames["tiaacref"] = "TIAA-CREF";
    d->m_fqNames["trustnet"] = "Trustnet";
    d->m_fqNames["uk_unit_trusts"] = "U.K. Unit Trusts";
    d->m_fqNames["unionfunds"] = "Union Investments";
    d->m_fqNames["tsp"] = "US Govt. Thrift Savings Plan";
    d->m_fqNames["usfedbonds"] = "US Treasury Bonds";
    d->m_fqNames["usa"] = "USA (Yahoo, Fool ...)";
    d->m_fqNames["vanguard"] = "Vanguard";
    d->m_fqNames["vwd"] = "VWD";
    d->m_fqNames["yahoo"] = "Yahoo";
    d->m_fqNames["yahoo_asia"] = "Yahoo Asia";
    d->m_fqNames["yahoo_australia"] = "Yahoo Australia";
    d->m_fqNames["yahoo_brasil"] = "Yahoo Brasil";
    d->m_fqNames["yahoo_europe"] = "Yahoo Europe";
    d->m_fqNames["yahoo_nz"] = "Yahoo New Zealand";
    d->m_fqNames["zifunds"] = "Zuerich Investments";
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReceivedDataFromFilter()));
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slotProcessExited()));
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(slotProcessExited()));
}

void AlkFinanceQuoteProcess::slotReceivedDataFromFilter()
{
    QByteArray data(readAllStandardOutput());

//   kDebug(2) << "WebPriceQuoteProcess::slotReceivedDataFromFilter(): " << QString(data);
    d->m_string += QString(data);
}

void AlkFinanceQuoteProcess::slotProcessExited()
{
//   kDebug(2) << "WebPriceQuoteProcess::slotProcessExited()";
    d->m_isDone = true;
}

void AlkFinanceQuoteProcess::launch(const QString &scriptPath)
{
    clearProgram();

    *this << "perl" << scriptPath << "-l";
    setOutputChannelMode(KProcess::OnlyStdoutChannel);
    start();
    if (!waitForStarted()) {
        qWarning("Unable to start FQ script");
    }
}

bool AlkFinanceQuoteProcess::isFinished() const
{
    return d->m_isDone;
}

const QStringList AlkFinanceQuoteProcess::getSourceList() const
{
    QStringList raw = d->m_string.split(0x0A, QString::SkipEmptyParts);
    QStringList sources;
    QStringList::iterator it;
    for (it = raw.begin(); it != raw.end(); ++it) {
        if (d->m_fqNames[*it].isEmpty()) {
            sources.append(*it);
        } else {
            sources.append(d->m_fqNames[*it]);
        }
    }
    sources.sort();
    return sources;
}

const QString AlkFinanceQuoteProcess::crypticName(const QString &niceName) const
{
    QString ret(niceName);
    Private::fqNameMap::const_iterator it;
    for (it = d->m_fqNames.begin(); it != d->m_fqNames.end(); ++it) {
        if (niceName == it.value()) {
            ret = it.key();
            break;
        }
    }
    return ret;
}

const QString AlkFinanceQuoteProcess::niceName(const QString &crypticName) const
{
    QString ret(d->m_fqNames[crypticName]);
    if (ret.isEmpty()) {
        ret = crypticName;
    }
    return ret;
}
