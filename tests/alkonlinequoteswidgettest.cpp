/*
    SPDX-FileCopyrightText: 2019 Ralf Habacker ralf.habacker @freenet.de
    SPDX-FileCopyrightText: 2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequoteswidget.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"

#include <QApplication>
#include <QDialog>
#include <QGridLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlkOnlineQuotesProfile profile("no-config-file", AlkOnlineQuotesProfile::Type::None);
    AlkOnlineQuotesProfileManager::instance().addProfile(&profile);

    QDialog dialog;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(new AlkOnlineQuotesWidget);
    dialog.setLayout(layout);
    dialog.show();
    app.exec();
}
