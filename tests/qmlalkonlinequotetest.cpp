/***************************************************************************
 *   Copyright 2018  Ralf Habacker ralf.habacker@freenet.de                *
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

#include "alkonlinequote.h"

#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#else
#include <QApplication>
class QGuiApplication : public QApplication
{
public:
    QGuiApplication(int &argc, char **argv) : QApplication(argc, argv) {}
};

#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFile>

class QQmlApplicationEngine : public QDeclarativeEngine
{
public:
    void load(const QString &url)
    {
        QString s = url;
        s.replace(".qml", "-qt4.qml");
        QUrl a = QUrl::fromLocalFile(s);
        view.setSource(a);
        view.show();
    }
    QDeclarativeView view;
};
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(CMAKE_CURRENT_SOURCE_DIR "/qmlalkonlinequotetest.qml");

    return app.exec();
}
