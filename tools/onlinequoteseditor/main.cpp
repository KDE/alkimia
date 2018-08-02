/***************************************************************************
 *   Copyright 2018 Ralf Habacker <ralf.habacker@freenet.de>               *
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

#include "mainwindow.h"

#if QT_VERSION >= 0x050000
#define KABOUTDATA_H
#include <K4AboutData>
#define KAboutData K4AboutData
#else
#include <KAboutData>
#endif

#include <KApplication>
#include <KCmdLineArgs>

int main(int argc, char **argv)
{
    KAboutData about("onlinequoteseditor", 0,
                     ki18n("onlinequoteseditor"),
                     "1.0",
                     ki18n("Editor for online price quotes used by finance applications"),
                     KAboutData::License_GPL,
                     ki18n("(C) 2018 Ralf Habacker"));
    KCmdLineArgs::init(argc, argv, &about);

    KApplication app(true);
    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);   // Add my own options.

    KComponentData a(&about);

    // Get application specific arguments
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    MainWindow w;
    w.show();
    return app.exec();
}
