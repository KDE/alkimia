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

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
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
    KAboutData about("onlinequoteseditor",
                     "onlinequoteeditor",
                     ki18n("onlinequoteseditor"),
                     "1.0",
                     ki18n("Editor for online price quotes used by finance applications"),
                     KAboutData::License_GPL,
                     ki18n("(C) 2018 Ralf Habacker"));
    KCmdLineArgs::init(argc, argv, &about);

    KApplication app(true);

    MainWindow w;
    w.show();
    return app.exec();
}
