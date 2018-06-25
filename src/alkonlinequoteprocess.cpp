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
  connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slotProcessExited(int,QProcess::ExitStatus)));
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

//
// Helper class to babysit the KProcess used for running the Finance Quote sources script
//

AlkFinanceQuoteProcess::AlkFinanceQuoteProcess()
{
  m_isDone = false;
  m_string = "";
  m_fqNames["aex"] = "AEX";
  m_fqNames["aex_futures"] = "AEX Futures";
  m_fqNames["aex_options"] = "AEX Options";
  m_fqNames["amfiindia"] = "AMFI India";
  m_fqNames["asegr"] = "ASE";
  m_fqNames["asia"] = "Asia (Yahoo, ...)";
  m_fqNames["asx"] = "ASX";
  m_fqNames["australia"] = "Australia (ASX, Yahoo, ...)";
  m_fqNames["bmonesbittburns"] = "BMO NesbittBurns";
  m_fqNames["brasil"] = "Brasil (Yahoo, ...)";
  m_fqNames["canada"] = "Canada (Yahoo, ...)";
  m_fqNames["canadamutual"] = "Canada Mutual (Fund Library, ...)";
  m_fqNames["deka"] = "Deka Investments";
  m_fqNames["dutch"] = "Dutch (AEX, ...)";
  m_fqNames["dwsfunds"] = "DWS";
  m_fqNames["europe"] = "Europe (Yahoo, ...)";
  m_fqNames["fidelity"] = "Fidelity (Fidelity, ...)";
  m_fqNames["fidelity_direct"] = "Fidelity Direct";
  m_fqNames["financecanada"] = "Finance Canada";
  m_fqNames["ftportfolios"] = "First Trust (First Trust, ...)";
  m_fqNames["ftportfolios_direct"] = "First Trust Portfolios";
  m_fqNames["fundlibrary"] = "Fund Library";
  m_fqNames["greece"] = "Greece (ASE, ...)";
  m_fqNames["indiamutual"] = "India Mutual (AMFI, ...)";
  m_fqNames["maninv"] = "Man Investments";
  m_fqNames["fool"] = "Motley Fool";
  m_fqNames["nasdaq"] = "Nasdaq (Yahoo, ...)";
  m_fqNames["nz"] = "New Zealand (Yahoo, ...)";
  m_fqNames["nyse"] = "NYSE (Yahoo, ...)";
  m_fqNames["nzx"] = "NZX";
  m_fqNames["platinum"] = "Platinum Asset Management";
  m_fqNames["seb_funds"] = "SEB";
  m_fqNames["sharenet"] = "Sharenet";
  m_fqNames["za"] = "South Africa (Sharenet, ...)";
  m_fqNames["troweprice_direct"] = "T. Rowe Price";
  m_fqNames["troweprice"] = "T. Rowe Price";
  m_fqNames["tdefunds"] = "TD Efunds";
  m_fqNames["tdwaterhouse"] = "TD Waterhouse Canada";
  m_fqNames["tiaacref"] = "TIAA-CREF";
  m_fqNames["trustnet"] = "Trustnet";
  m_fqNames["uk_unit_trusts"] = "U.K. Unit Trusts";
  m_fqNames["unionfunds"] = "Union Investments";
  m_fqNames["tsp"] = "US Govt. Thrift Savings Plan";
  m_fqNames["usfedbonds"] = "US Treasury Bonds";
  m_fqNames["usa"] = "USA (Yahoo, Fool ...)";
  m_fqNames["vanguard"] = "Vanguard";
  m_fqNames["vwd"] = "VWD";
  m_fqNames["yahoo"] = "Yahoo";
  m_fqNames["yahoo_asia"] = "Yahoo Asia";
  m_fqNames["yahoo_australia"] = "Yahoo Australia";
  m_fqNames["yahoo_brasil"] = "Yahoo Brasil";
  m_fqNames["yahoo_europe"] = "Yahoo Europe";
  m_fqNames["yahoo_nz"] = "Yahoo New Zealand";
  m_fqNames["zifunds"] = "Zuerich Investments";
  connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReceivedDataFromFilter()));
  connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(slotProcessExited()));
  connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(slotProcessExited()));
}

void AlkFinanceQuoteProcess::slotReceivedDataFromFilter()
{
  QByteArray data(readAllStandardOutput());

//   kDebug(2) << "WebPriceQuoteProcess::slotReceivedDataFromFilter(): " << QString(data);
  m_string += QString(data);
}

void AlkFinanceQuoteProcess::slotProcessExited()
{
//   kDebug(2) << "WebPriceQuoteProcess::slotProcessExited()";
  m_isDone = true;
}

void AlkFinanceQuoteProcess::launch(const QString& scriptPath)
{
  clearProgram();

  *this << "perl" << scriptPath << "-l";
  setOutputChannelMode(KProcess::OnlyStdoutChannel);
  start();
  if (! waitForStarted()) qWarning("Unable to start FQ script");
  return;
}

const QStringList AlkFinanceQuoteProcess::getSourceList() const
{
  QStringList raw = m_string.split(0x0A, QString::SkipEmptyParts);
  QStringList sources;
  QStringList::iterator it;
  for (it = raw.begin(); it != raw.end(); ++it) {
    if (m_fqNames[*it].isEmpty()) sources.append(*it);
    else sources.append(m_fqNames[*it]);
  }
  sources.sort();
  return (sources);
}

const QString AlkFinanceQuoteProcess::crypticName(const QString& niceName) const
{
  QString ret(niceName);
  fqNameMap::const_iterator it;
  for (it = m_fqNames.begin(); it != m_fqNames.end(); ++it) {
    if (niceName == it.value()) {
      ret = it.key();
      break;
    }
  }
  return (ret);
}

const QString AlkFinanceQuoteProcess::niceName(const QString& crypticName) const
{
  QString ret(m_fqNames[crypticName]);
  if (ret.isEmpty()) ret = crypticName;
  return (ret);
}
