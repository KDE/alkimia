/*
    SPDX-FileCopyrightText: 2004-2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkonlinequoteswidget.h"

#include "alkonlinequote.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"
#include "alkwebpage.h"

#include <QRegExp>
#include <QCheckBox>
#include <QDesktopServices>
#include <QtDebug>
#include <QTreeWidget>
#include <QKeyEvent>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    #include <QIcon>
    #include <KIconLoader>
#include <knewstuff_version.h>
#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
    #include <kns3/downloaddialog.h>
#else
    #include <KNS3/QtQuickDialogWrapper>
#endif
    #define KIcon QIcon
#else
    #include <KComponentData>
    #include <KIcon>
    #include <KIconLoader>
    #include <knewstuff3/downloaddialog.h>
#define KNEWSTUFF_VERSION 0
#endif

#include <KGuiItem>
#include <KMessageBox>

#include <ui_alkonlinequoteswidget.h>

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
#include <klocale.h>
static KLocale _locale(TRANSLATION_DOMAIN);
#define i18nc(context, text) ki18nc(context, text).toString(&_locale)
#define i18n(text) ki18n(text).toString(&_locale)
#define tr2i18n(text, context) ki18nc(context, text).toString(&_locale)
#endif

class AlkOnlineQuotesWidget::Private : public QWidget, public Ui::AlkOnlineQuotesWidget
{
    Q_OBJECT
public:
    QString m_acceptLanguage;
    QList<AlkOnlineQuoteSource> m_resetList;
    AlkOnlineQuoteSource m_currentItem;
    bool m_quoteInEditing;
    AlkOnlineQuotesProfile *m_profile;
    bool m_showProfiles;
    bool m_showUpload;
    QPixmap m_emptyIcon;
    QPixmap m_inWorkIcon;
    QPixmap m_okIcon;
    QPixmap m_failIcon;
    QPixmap m_unknownIcon;
    QDialog *m_webPageDialog;

    Private(bool showProfiles, bool showUpload, QWidget *parent);
    ~Private();
public Q_SLOTS:
    void slotNewProfile();
    void slotDeleteProfile();
    void slotSelectProfile();
    void slotLoadProfile();

    void slotDeleteEntry();
    void slotDuplicateEntry();
    void slotUpdateEntry();
    void slotLoadQuoteSource();
    void slotEntryChanged();
    void slotNewEntry();
    void slotCheckEntry();
    void slotLogStatus(const QString &s);
    void slotLogError(const QString &s);
    void slotLogFailed(const QString &id, const QString &symbol);
    void slotLogQuote(const QString &id, const QString &symbol, const QDate &date, double price);
    void slotQuoteSourceRenamed(QTreeWidgetItem *item, int column);
    void slotQuoteSourceStartRename(QTreeWidgetItem *item, int column);
    void slotInstallEntries();
    void slotShowButton();

public:
    void loadProfiles();
    void loadQuotesList(const bool updateResetList = false);
    void clearIcons();
    void initIcons();
    void setupIcons(const AlkOnlineQuote::Errors &errors);
    QString singleSymbol() const;
    QStringList doubleSymbol() const;
    QString expandedUrl() const;
    void updateButtonState();
    bool eventFilter(QObject* o, QEvent* e) override;
};

AlkOnlineQuotesWidget::Private::Private(bool showProfiles, bool showUpload, QWidget *parent)
    : QWidget(parent)
    , m_quoteInEditing(false)
    , m_profile(nullptr)
    , m_showProfiles(showProfiles)
    , m_showUpload(showUpload)
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    , m_inWorkIcon(BarIcon("view-refresh"))
    , m_okIcon(BarIcon("dialog-ok-apply"))
    , m_failIcon(BarIcon("dialog-cancel"))
#else
    , m_inWorkIcon(QIcon::fromTheme("view-refresh").pixmap(16))
    , m_okIcon(QIcon::fromTheme("dialog-ok-apply").pixmap(16))
    , m_failIcon(QIcon::fromTheme("dialog-cancel").pixmap(16))
#endif
    , m_webPageDialog(nullptr)
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    static KComponentData alk(TRANSLATION_DOMAIN);
#endif
    setupUi(parent);

    profilesGroupBox->setVisible(showProfiles);
    profileDetailsBox->setVisible(showProfiles);
    m_showButton->setVisible(!showProfiles && AlkOnlineQuotesProfileManager::instance().webPageEnabled());
    m_ghnsSource->setEnabled(showProfiles);
    m_urlCheckLabel->setMinimumWidth(m_okIcon.width());

    loadProfiles();

    // TODO move to ui file
    KGuiItem updateButtenItem(i18nc("Accepts the entered data and stores it", "&Accept"),
                              "dialog-ok",
                              i18n("Accepts the entered data and stores it"),
                              i18n("Use this to accept the modified data."));

    KGuiItem deleteButtenItem(i18n("&Delete"),
                              "edit-delete",
                              i18n("Delete the selected source entry"),
                              i18n("Use this to delete the selected online source entry"));

    KGuiItem checkButtonItem(i18nc("Check the selected source entry", "&Check Source"),
                             "document-edit-verify",
                             i18n("Check the selected source entry"),
                             i18n("Use this to check the selected online source entry"));

    KGuiItem showButtonItem(i18nc("Show the selected source entry in a web browser", "&Show page"),
                            "applications-internet",
                            i18n("Show the selected source entry in a web browser"),
                            i18n("Use this to show the selected online source entry"));

    KGuiItem newButtenItem(i18nc("Create a new source entry for online quotes", "&New..."),
                           "document-new",
                           i18n("Create a new source entry for online quotes"),
                           i18n("Use this to create a new entry for online quotes"));

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    KGuiItem::assign(m_updateButton, updateButtenItem);
    KGuiItem::assign(m_deleteButton, deleteButtenItem);
    KGuiItem::assign(m_checkButton, checkButtonItem);
    KGuiItem::assign(m_showButton, showButtonItem);
    KGuiItem::assign(m_newButton, newButtenItem);
#else
    m_updateButton->setGuiItem(updateButtenItem);
    m_deleteButton->setGuiItem(deleteButtenItem);
    m_checkButton->setGuiItem(checkButtonItem);
    m_showButton->setGuiItem(showButtonItem);
    m_newButton->setGuiItem(newButtenItem);
#endif

    connect(m_newProfile, SIGNAL(clicked()), this, SLOT(slotNewProfile()));
    connect(m_deleteProfile, SIGNAL(clicked()), this, SLOT(slotDeleteProfile()));
    connect(m_profileList, SIGNAL(itemSelectionChanged()), this, SLOT(slotLoadProfile()));

    connect(m_updateButton, SIGNAL(clicked()), this, SLOT(slotUpdateEntry()));
    connect(m_newButton, SIGNAL(clicked()), this, SLOT(slotNewEntry()));
    connect(m_checkButton, SIGNAL(clicked()), this, SLOT(slotCheckEntry()));
    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
    connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(slotDuplicateEntry()));
    connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstallEntries()));

    m_quoteSourceList->setColumnCount(2);
    m_quoteSourceList->setHeaderLabels(QStringList() << i18n("Name") << i18n("Source"));
    m_quoteSourceList->setRootIsDecorated(false);
    m_quoteSourceList->header()->resizeSection(1, 5);
    m_quoteSourceList->header()->setStretchLastSection(false);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    m_quoteSourceList->header()->setSectionResizeMode(0,QHeaderView::Stretch);
#else
    m_quoteSourceList->header()->setResizeMode(0,QHeaderView::Stretch);
#endif
    m_quoteSourceList->setSortingEnabled(true);
    m_quoteSourceList->installEventFilter(this);

    connect(m_quoteSourceList, SIGNAL(itemSelectionChanged()), this, SLOT(slotLoadQuoteSource()));
    connect(m_quoteSourceList, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
            SLOT(slotQuoteSourceRenamed(QTreeWidgetItem *, int)));
    connect(m_quoteSourceList, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this,
            SLOT(slotQuoteSourceStartRename(QTreeWidgetItem *, int)));

    connect(m_editURL, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editSymbol, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editDate, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editDateFormat, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editPrice, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_skipStripping, SIGNAL(toggled(bool)), this, SLOT(slotEntryChanged()));
    connect(m_ghnsSource, SIGNAL(toggled(bool)), this, SLOT(slotEntryChanged()));
    connect(m_showButton, SIGNAL(clicked()), this, SLOT(slotShowButton()));

    m_checkSymbol->setText("ORCL");
    m_checkSymbol2->setText("BTC GBP");
    m_updateButton->setEnabled(false);
    slotLoadProfile();
}

AlkOnlineQuotesWidget::Private::~Private()
{
    m_webPageDialog->deleteLater();
}

void AlkOnlineQuotesWidget::Private::loadProfiles()
{
    AlkOnlineQuotesProfileList list = AlkOnlineQuotesProfileManager::instance().profiles();
    if (list.isEmpty())
        return;
    for (AlkOnlineQuotesProfile *profile : list) {
        QListWidgetItem *item = new QListWidgetItem(dynamic_cast<QListWidget *>(m_profileList));
        item->setText(profile->name());
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
    m_profileList->setCurrentRow(0);
    m_profile = AlkOnlineQuotesProfileManager::instance().profiles().first();
    loadQuotesList();
}

QString sourceTypeString(AlkOnlineQuoteSource &source)
{
    if (source.isGHNS())
        return i18n("GHNS Source");
    else if (source.isFinanceQuote())
        return i18n("Finance::Quote");
    return i18n("local");
}

void AlkOnlineQuotesWidget::Private::loadQuotesList(const bool updateResetList)
{
    m_quoteInEditing = false;
    QStringList groups = m_profile->quoteSources();

    if (updateResetList) {
        m_resetList.clear();
    }
    m_quoteSourceList->blockSignals(true);
    m_quoteSourceList->clear();
    QStringList::Iterator it;
    for (it = groups.begin(); it != groups.end(); ++it) {
        AlkOnlineQuoteSource source(*it, m_profile);
        if (!source.isValid()) {
            continue;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << *it << sourceTypeString(source));
        item->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_quoteSourceList->addTopLevelItem(item);
        if (updateResetList) {
            m_resetList += source;
        }
    }
    m_quoteSourceList->sortItems(0, Qt::AscendingOrder);

    QTreeWidgetItem *item = nullptr;
    if (!m_currentItem.name().isEmpty()) {
        QList<QTreeWidgetItem*> items = m_quoteSourceList->findItems(m_currentItem.name(), Qt::MatchExactly);
        if (items.size() > 0)
            item = items.at(0);
        if (item)
            m_quoteSourceList->setCurrentItem(item);
    }
    if (!item) {
        item = m_quoteSourceList->topLevelItem(0);
        if (item)
            m_quoteSourceList->setCurrentItem(item);
    }
    m_quoteSourceList->blockSignals(false);
    slotLoadQuoteSource();
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotNewProfile()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(dynamic_cast<QTreeWidget *>(m_profileList));
    item->setText(0, QLatin1String("new profile"));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
}

void AlkOnlineQuotesWidget::Private::slotDeleteProfile()
{
    delete m_profileList->currentItem();
}

void AlkOnlineQuotesWidget::Private::slotSelectProfile()
{
    slotLoadProfile();
}

void AlkOnlineQuotesWidget::Private::slotLoadProfile()
{
    AlkOnlineQuotesProfileList list = AlkOnlineQuotesProfileManager::instance().profiles();
    if (!m_showProfiles) {
        AlkOnlineQuotesProfileList list = AlkOnlineQuotesProfileManager::instance().profiles();
        if (list.isEmpty())
            return;
        m_profile = list.first();
        m_installButton->setVisible(m_profile->hasGHNSSupport());
        loadQuotesList();
        return;
    }

    for (AlkOnlineQuotesProfile *profile : list) {
        if (m_profileList->currentItem() && m_profileList->currentItem()->text() == profile->name()) {
            m_profile = profile;
            loadQuotesList();
            m_installButton->setVisible(profile->hasGHNSSupport());
        }
    }

    bool visible = m_profile->type() != AlkOnlineQuotesProfile::Type::None;
    m_configFilePath->setText(m_profile->kConfigFile());
    m_configFilePath->setVisible(visible);
    m_configLabel->setEnabled(visible);

    visible = m_profile->hasGHNSSupport();
    m_GHNSConfigFilePath->setText(m_profile->hotNewStuffConfigFile());
    m_GHNSConfigFilePath->setVisible(visible);
    m_GHNSConfigLabel->setEnabled(visible);

    m_GHNSDataPath->setText(m_profile->hotNewStuffReadPath().join(" "));
    m_GHNSDataPath->setVisible(visible);
    m_GHNSDataLabel->setEnabled(visible);
}

void AlkOnlineQuotesWidget::Private::slotLoadQuoteSource()
{
    m_quoteInEditing = false;
    QTreeWidgetItem *item = m_quoteSourceList->currentItem();
    bool isFinanceQuoteSource = (item && AlkOnlineQuoteSource::isFinanceQuote(item->text(0))) ||
            m_profile->type() == AlkOnlineQuotesProfile::Type::Script;
    bool enabled = item && !isFinanceQuoteSource;

    m_editURL->setEnabled(enabled);
    m_editSymbol->setEnabled(enabled);
    m_editPrice->setEnabled(enabled);
    m_editDate->setEnabled(enabled);
    m_editDateFormat->setEnabled(enabled);
    m_ghnsSource->setEnabled(!isFinanceQuoteSource);
    m_skipStripping->setEnabled(enabled);

    m_editURL->clear();
    m_editSymbol->clear();
    m_editPrice->clear();
    m_editDate->clear();
    m_editDateFormat->clear();

    if (item) {
        m_currentItem = AlkOnlineQuoteSource(item->text(0), m_profile);
        m_editURL->setText(m_currentItem.url());
        m_editSymbol->setText(m_currentItem.idNumber());
        m_editPrice->setText(m_currentItem.priceRegex());
        m_editDate->setText(m_currentItem.date());
        m_editDateFormat->setText(m_currentItem.dateformat());
        m_skipStripping->setChecked(m_currentItem.skipStripping());
        m_ghnsSource->setChecked(m_currentItem.isGHNS());
    }

    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotEntryChanged()
{
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::updateButtonState()
{
    clearIcons();
    bool modified = m_editURL->text() != m_currentItem.url()
                    || m_editSymbol->text() != m_currentItem.idNumber()
                    || m_editDate->text() != m_currentItem.date()
                    || m_editDateFormat->text() != m_currentItem.dateformat()
                    || m_editPrice->text() != m_currentItem.priceRegex()
                    || m_skipStripping->isChecked() != m_currentItem.skipStripping()
                    || m_ghnsSource->isChecked() != m_currentItem.isGHNS();

    bool isFinanceQuote = m_currentItem.isFinanceQuote() || m_profile->type() == AlkOnlineQuotesProfile::Type::Script;
    bool hasWriteSupport = (m_profile->type() != AlkOnlineQuotesProfile::Type::None && !isFinanceQuote) || m_profile->hasGHNSSupport();
    bool noNewEntry = m_quoteSourceList->findItems(i18n("New Quote Source"), Qt::MatchExactly).count() == 0 || isFinanceQuote;
    m_newButton->setEnabled(hasWriteSupport && noNewEntry);
    m_duplicateButton->setEnabled(hasWriteSupport);
    m_deleteButton->setEnabled(!m_currentItem.isReadOnly() && !m_currentItem.isGHNS());
    m_updateButton->setEnabled(modified);
    m_checkButton->setEnabled(isFinanceQuote || !modified);
    m_checkSymbol->setEnabled(!m_currentItem.url().contains("%2"));
    m_checkSymbol2->setEnabled(m_currentItem.url().contains("%2"));
}

void AlkOnlineQuotesWidget::Private::slotDeleteEntry()
{
    QList<QTreeWidgetItem *> items = m_quoteSourceList->findItems(
        m_currentItem.name(), Qt::MatchExactly);
    if (items.isEmpty()) {
        return;
    }
    QTreeWidgetItem *item = items.at(0);
    if (!item) {
        return;
    }

    int ret = KMessageBox::warningContinueCancel(this,
                                                 i18n("Are you sure to delete this online quote ?"),
                                                 i18n("Delete online quote"),
                                                 KStandardGuiItem::cont(),
                                                 KStandardGuiItem::cancel(),
                                                 QString("DeletingOnlineQuote"));
    if (ret == KMessageBox::Cancel) {
        return;
    }

    // keep this order to avoid deleting the wrong current item
    m_currentItem.remove();
    delete item;
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotDuplicateEntry()
{
    QList<QTreeWidgetItem *> items = m_quoteSourceList->findItems(
        m_currentItem.name(), Qt::MatchExactly);
    if (items.isEmpty()) {
        return;
    }
    QTreeWidgetItem *item = items.at(0);
    if (!item) {
        return;
    }

    AlkOnlineQuoteSource copy(m_currentItem);
    copy.setName(copy.name() + i18n(".copy"));
    copy.setGHNS(false);
    copy.write();
    m_currentItem = copy;
    loadQuotesList();
}

void AlkOnlineQuotesWidget::Private::slotUpdateEntry()
{
    m_currentItem.setUrl(m_editURL->text());
    m_currentItem.setIdNumber(m_editSymbol->text());
    m_currentItem.setDate(m_editDate->text());
    m_currentItem.setDateformat(m_editDateFormat->text());
    m_currentItem.setPriceRegex(m_editPrice->text());
    m_currentItem.setSkipStripping(m_skipStripping->isChecked());
    m_currentItem.setGHNS(m_ghnsSource->isChecked());
    m_currentItem.write();
    m_checkButton->setEnabled(true);
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotNewEntry()
{
    AlkOnlineQuoteSource newSource(i18n("New Quote Source"), m_profile);
    newSource.write();
    m_currentItem = newSource;
    loadQuotesList();
}

void AlkOnlineQuotesWidget::Private::clearIcons()
{
    m_urlCheckLabel->setPixmap(m_emptyIcon);
    m_dateCheckLabel->setPixmap(m_emptyIcon);
    m_priceCheckLabel->setPixmap(m_emptyIcon);
    m_symbolCheckLabel->setPixmap(m_emptyIcon);
    m_dateFormatCheckLabel->setPixmap(m_emptyIcon);
}

void AlkOnlineQuotesWidget::Private::initIcons()
{
    m_urlCheckLabel->setPixmap(m_inWorkIcon);
    m_dateCheckLabel->setPixmap(m_inWorkIcon);
    m_priceCheckLabel->setPixmap(m_inWorkIcon);
    m_symbolCheckLabel->setPixmap(m_inWorkIcon);
    m_dateFormatCheckLabel->setPixmap(m_inWorkIcon);
}

void AlkOnlineQuotesWidget::Private::setupIcons(const AlkOnlineQuote::Errors &errors)
{
    clearIcons();
    if (errors & AlkOnlineQuote::Errors::URL) {
        m_urlCheckLabel->setPixmap(m_failIcon);
    } else {
        m_urlCheckLabel->setPixmap(m_okIcon);
        m_symbolCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::Symbol ? m_failIcon : m_okIcon);
        m_priceCheckLabel->setPixmap(errors & AlkOnlineQuote::Errors::Price ? m_failIcon : m_okIcon);
        if (errors & AlkOnlineQuote::Errors::Date) {
            m_dateCheckLabel->setPixmap(m_failIcon);
        } else {
            if (m_currentItem.date().isEmpty()) {
                m_dateCheckLabel->setPixmap(m_emptyIcon);
                m_dateFormatCheckLabel->setPixmap(m_emptyIcon);
            } else {
                m_dateCheckLabel->setPixmap(m_okIcon);
                m_dateFormatCheckLabel->setPixmap(
                    errors & AlkOnlineQuote::Errors::DateFormat ? m_failIcon : m_okIcon);
            }
        }
    }
}

void AlkOnlineQuotesWidget::Private::slotCheckEntry()
{
    AlkOnlineQuote quote(m_profile);
    m_logWindow->setVisible(true);
    m_logWindow->clear();
    clearIcons();
    quote.setAcceptLanguage(m_acceptLanguage);

    connect(&quote, SIGNAL(status(QString)), this, SLOT(slotLogStatus(QString)));
    connect(&quote, SIGNAL(error(QString)), this, SLOT(slotLogError(QString)));
    connect(&quote, SIGNAL(failed(QString,QString)), this, SLOT(slotLogFailed(QString,QString)));
    connect(&quote, SIGNAL(quote(QString,QString,QDate,double)), this,
            SLOT(slotLogQuote(QString,QString,QDate,double)));
    initIcons();
    if (m_currentItem.url().contains("%2")) {
        quote.launch(m_checkSymbol2->text(), m_checkSymbol2->text(), m_currentItem.name());
    } else {
        quote.launch(m_checkSymbol->text(), m_checkSymbol->text(), m_currentItem.name());
    }
    setupIcons(quote.errors());
}

void AlkOnlineQuotesWidget::Private::slotLogStatus(const QString &s)
{
    m_logWindow->append(s);
}

void AlkOnlineQuotesWidget::Private::slotLogError(const QString &s)
{
    slotLogStatus(QString("<font color=\"red\"><b>") + s + QString("</b></font>"));
}

void AlkOnlineQuotesWidget::Private::slotLogFailed(const QString &id, const QString &symbol)
{
    slotLogStatus(QString("%1 %2").arg(id, symbol));
}

void AlkOnlineQuotesWidget::Private::slotLogQuote(const QString &id, const QString &symbol,
                                         const QDate &date, double price)
{
    slotLogStatus(QString("<font color=\"green\">%1 %2 %3 %4</font>").arg(id, symbol,
                                                                          date.toString()).arg(
                      price));
}

void AlkOnlineQuotesWidget::Private::slotQuoteSourceStartRename(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    m_quoteInEditing = true;
    m_quoteSourceList->editItem(item);
}

void AlkOnlineQuotesWidget::Private::slotQuoteSourceRenamed(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    //if there is no current item selected, exit
    if (m_quoteInEditing == false || !m_quoteSourceList->currentItem()
        || item != m_quoteSourceList->currentItem()) {
        // revert name change
        item->setText(0, m_currentItem.name());
        return;
    }

    m_quoteInEditing = false;
    QString text = item->text(0);
    int nameCount = 0;
    for (int i = 0; i < m_quoteSourceList->topLevelItemCount(); ++i) {
        if (m_quoteSourceList->topLevelItem(i)->text(0) == text) {
            ++nameCount;
        }
    }

    // Make sure we get a non-empty and unique name
    if (text.length() > 0 && nameCount == 1) {
        m_currentItem.rename(text);
    } else {
        // revert name change
        item->setText(0, m_currentItem.name());
    }
    m_quoteSourceList->sortItems(0, Qt::AscendingOrder);
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotInstallEntries()
{
    QString configFile = m_profile->hotNewStuffConfigFile();

#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 78, 0)
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog(configFile, this);
    dialog->exec();
    delete dialog;
    loadQuotesList();
#else
#if KNEWSTUFF_VERSION < QT_VERSION_CHECK(5, 94, 0)
    if (!KNS3::QtQuickDialogWrapper(configFile).exec().isEmpty()) {
        // Only load the list if entries are changed
        loadQuotesList();
    }
#else
    auto knsWrapper = new KNS3::QtQuickDialogWrapper(configFile, this);
    connect(knsWrapper, &KNS3::QtQuickDialogWrapper::closed, this, [knsWrapper, this](){
        if (!knsWrapper->changedEntries().isEmpty()) {
            // Only load the list if entries are changed
            loadQuotesList();
        }
        knsWrapper->deleteLater();
    });
    knsWrapper->open();
#endif
#endif
}

void AlkOnlineQuotesWidget::Private::slotShowButton()
{
    if (!m_webPageDialog) {
        m_webPageDialog = new QDialog;
        m_webPageDialog->setWindowTitle(i18n("Online Quote HTML Result Window"));
        QVBoxLayout *layout = new QVBoxLayout;
        AlkWebPage *webPage = AlkOnlineQuotesProfileManager::instance().webPage();
        webPage->setWebInspectorEnabled(true);
        layout->addWidget(webPage->widget());
        m_webPageDialog->setLayout(layout);
    }
    m_webPageDialog->show();
}

QString AlkOnlineQuotesWidget::Private::expandedUrl() const
{
    if (m_currentItem.url().contains("%2")) {
        return m_currentItem.url().arg(m_checkSymbol2->text());
    } else {
        return m_currentItem.url().arg(m_checkSymbol->text());
    }
}

bool AlkOnlineQuotesWidget::Private::eventFilter(QObject* o, QEvent* e)
{
    if (o == m_quoteSourceList && e->type() == QEvent::KeyRelease) {
        QKeyEvent *k = dynamic_cast<QKeyEvent*>(e);
#ifdef Q_OS_DARWIN
        if (k->key() == Qt::Key_Enter) {
#else
        if (k->key() == Qt::Key_F2) {
#endif
            slotQuoteSourceStartRename(m_quoteSourceList->currentItem(), 0);
        }
    }
    return false;
}

AlkOnlineQuotesWidget::AlkOnlineQuotesWidget(bool showProfiles, bool showUpload, QWidget *parent)
    : QWidget(parent)
    , d(new Private(showProfiles, showUpload, this))
{
}

AlkOnlineQuotesWidget::~AlkOnlineQuotesWidget()
{
    delete d;
}

QWidget *AlkOnlineQuotesWidget::profilesWidget()
{
    QFrame *frame = new QFrame;
    frame->setLayout(d->profilesGroupBox->layout());
    return frame;
}

QWidget *AlkOnlineQuotesWidget::profileDetailsWidget()
{
    QFrame *frame = new QFrame;
    frame->setLayout(d->profileDetailsBox->layout());
    return frame;
}

QWidget *AlkOnlineQuotesWidget::onlineQuotesWidget()
{
    QFrame *frame = new QFrame;
    frame->setLayout(d->onlineQuotesGroupBox->layout());
    return frame;
}

QWidget *AlkOnlineQuotesWidget::quoteDetailsWidget()
{
    QFrame *frame = new QFrame;
    frame->setLayout(d->detailsGroupBox->layout());
    return frame;
}

QWidget *AlkOnlineQuotesWidget::debugWidget()
{
    QFrame *frame = new QFrame;
    frame->setLayout(d->debugGroupBox->layout());
    return frame;
}

void AlkOnlineQuotesWidget::readConfig()
{
}

void AlkOnlineQuotesWidget::writeConfig()
{
}

void AlkOnlineQuotesWidget::resetConfig()
{
    QStringList::ConstIterator it;
    QStringList groups = d->m_profile->quoteSources();

    // delete all currently defined entries
    for (it = groups.constBegin(); it != groups.constEnd(); ++it) {
        AlkOnlineQuoteSource(*it, d->m_profile).remove();
    }

    // and write back the one's from the reset list
    QList<AlkOnlineQuoteSource>::iterator itr;
    for (itr = d->m_resetList.begin(); itr != d->m_resetList.end(); ++itr) {
        (*itr).write();
    }

    d->loadQuotesList();
}

QString AlkOnlineQuotesWidget::acceptLanguage() const
{
    return d->m_acceptLanguage;
}

void AlkOnlineQuotesWidget::setAcceptLanguage(const QString &text)
{
    d->m_acceptLanguage = text;
}

#include "alkonlinequoteswidget.moc"
