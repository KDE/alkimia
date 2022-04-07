/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequote.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
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
