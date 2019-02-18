/***************************************************************************
 *   Copyright 2019  Ralf Habacker ralf.habacker@freenet.de                *
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

#include "alkonlinequoteswidget.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"

#include <KComponentData>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include <QGuiApplication>
#else
#include <QApplication>
class QGuiApplication : public QApplication
{
public:
    QGuiApplication(int &argc, char **argv) : QApplication(argc, argv) {}
};
#endif

#include <QDialog>
#include <QGridLayout>

class Dialog : public QDialog {
public:
    Dialog()
    {

        QGridLayout *layout = new QGridLayout;
        layout->addWidget(new AlkOnlineQuotesWidget);
        setLayout(layout);
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    AlkOnlineQuotesProfile profile("no-config-file", AlkOnlineQuotesProfile::Type::None);
    AlkOnlineQuotesProfileManager::instance().addProfile(&profile);

    Dialog dialog;
    dialog.exec();
}
