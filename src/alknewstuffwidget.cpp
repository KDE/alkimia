/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alknewstuffwidget.h"

#include "alknewstuffentry_p.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #define KNEWSTUFFWIDGETS_PRIVATE_BUILDING
    #include <KNSWidgets/Action>
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <knscore/engine.h>
    #include <knewstuff_version.h>
#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
    #include <kns3/downloaddialog.h>
#else
    #include <KNS3/QtQuickDialogWrapper>
#endif
#else
    #include <knewstuff3/downloadmanager.h>
    #include <knewstuff3/downloaddialog.h>
#define KNEWSTUFF_VERSION 0
#endif

#include <QEventLoop>
#include <QPointer>
#include <QWidget>

class AlkNewStuffWidget::Private : public QObject
{
    Q_OBJECT
public:
    AlkNewStuffWidget *q;
    QString _configFile;
    Private(AlkNewStuffWidget *parent);

    ~Private();
};

AlkNewStuffWidget::Private::Private(AlkNewStuffWidget *parent)
    : q(parent)
{
}

AlkNewStuffWidget::Private::~Private()
{
}

AlkNewStuffWidget::AlkNewStuffWidget(QObject *parent)
    : QObject{parent}
    , d(new Private(this))
{
}

bool AlkNewStuffWidget::init(const QString &configFile)
{
    d->_configFile = configFile;
    return true;
}

bool AlkNewStuffWidget::showInstallDialog(QWidget *parent)
{
    QString configFile = d->_configFile;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointer<KNSWidgets::Action> knsWrapper = new KNSWidgets::Action("Install Online quotes", configFile, parent);
    QEventLoop *loop = new QEventLoop;
    QList<KNSCore::Entry> *entries = new QList<KNSCore::Entry>;
    connect(knsWrapper, &KNSWidgets::Action::dialogFinished, this, [this, entries, loop](const QList<KNSCore::Entry> &changedEntries)
    {
        *entries = changedEntries;
        loop->quit();
    });
    knsWrapper->trigger();
    loop->exec();
    delete loop;
    bool result = !entries->isEmpty();
    delete entries;
    return result;
#elif KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog(configFile, parent);
    dialog->exec();
    alkDebug() << "changed entries" << dialog->changedEntries();
    alkDebug() << "installed entries" << dialog->installedEntries();
    delete dialog;
    return true;
#elif KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 94, 0)
    return !KNS3::QtQuickDialogWrapper(configFile).exec().isEmpty();
#else
    QPointer<KNS3::QtQuickDialogWrapper> knsWrapper = new KNS3::QtQuickDialogWrapper(configFile, dynamic_cast<QObject*>(parent));
    knsWrapper->open();
    QEventLoop loop;
    connect(knsWrapper, &KNS3::QtQuickDialogWrapper::closed, &loop, &QEventLoop::quit);
    loop.exec();
    return !knsWrapper->changedEntries().empty();
#endif
}

#include "alknewstuffwidget.moc"
