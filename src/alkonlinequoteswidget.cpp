/*
    SPDX-FileCopyrightText: 2004-2019 Thomas Baumgart tbaumgart @kde.org

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkonlinequoteswidget.h"

#include "alkdebug.h"
#include "alknewstuffwidget.h"
#include "alkonlinequote.h"
#include "alkonlinequotesmodel.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"
#include "alkonlinequoteuploaddialog.h"
#include "alkwebpage.h"
#include "alkwebview.h"

#include <QCheckBox>
#include <QDesktopServices>
#include <QTreeWidget>
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QtDebug>
#include <QTreeView>
#include <QTreeWidgetItem>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QIcon>
    #include <KIconLoader>
    #include <KMessageWidget>
    #define KIcon QIcon
#else
    #include <KComponentData>
    #include <KIcon>
    #include <KIconLoader>
#endif

#if QT_VERSION > QT_VERSION_CHECK(5, 0, 0)
#include <QLocale>
#define initLocale() QLocale()
#else
#include <KGlobal>
#define initLocale() KGlobal::locale()
#endif

#include <KGuiItem>
#include <KMessageBox>

#include <ui_alkonlinequotedetails.h>
#include <ui_alkonlinequotes.h>
#include <ui_alkonlinequotesdebug.h>
#include <ui_alkonlinequotesprofiledetails.h>
#include <ui_alkonlinequotesprofiles.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <klocale.h>
static KLocale _locale(TRANSLATION_DOMAIN);
#define i18nc(context, text) ki18nc(context, text).toString(&_locale)
#define i18n(text) ki18n(text).toString(&_locale)
#define tr2i18n(text, context) ki18nc(context, text).toString(&_locale)
#endif

class AlkOnlineQuotesWidget::Private
    : public QWidget
    , public Ui::AlkOnlineQuoteDetailsWidget
    , public Ui::AlkOnlineQuotesDebugWidget
    , public Ui::AlkOnlineQuotesProfileDetailsWidget
    , public Ui::AlkOnlineQuotesProfilesWidget
    , public Ui::AlkOnlineQuotesWidget
{
    Q_OBJECT
public:
    QString m_acceptLanguage;
    QList<AlkOnlineQuoteSource> m_resetList;
    AlkOnlineQuoteSource m_currentItem;
    bool m_quoteInEditing;
    AlkOnlineQuotesProfile *m_profile;
    AlkWebView *m_webView;
    bool m_showProfiles;
    bool m_showUpload;
    bool m_ghnsEditable;
    bool m_disableUpdate;
    QPixmap m_emptyIcon;
    QPixmap m_inWorkIcon;
    QPixmap m_okIcon;
    QPixmap m_failIcon;
    QPixmap m_unknownIcon;
    QDialog *m_webPageDialog;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    KMessageWidget* m_infoMessage;
#endif
    AlkOnlineQuotesModel *m_model;

    Private(bool showProfiles, bool showUpload, QWidget *parent);
    ~Private();

public Q_SLOTS:
    void slotNewProfile();
    void slotDeleteProfile();
    void slotSelectProfile();
    void slotLoadProfile();

    void slotDeleteEntry();
    void slotDuplicateEntry();
    void slotAcceptEntry();
    void slotLoadQuoteSource(const QModelIndex &index = QModelIndex());
    void slotEntryChanged();
    void slotNewEntry();
    void slotCheckEntry();
    void slotLogStatus(const QString &s);
    void slotLogError(const QString &s);
    void slotLogFailed(const QString &id, const QString &symbol);
    void slotLogQuote(const QString &id, const QString &symbol, const QDate &date, double price);
    void slotLogQuotes(const QString &id, const QString &symbol, const AlkDatePriceMap &prices);
    void slotInstallEntries();
    void slotUploadEntry();
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
};

AlkOnlineQuotesWidget::Private::Private(bool showProfiles, bool showUpload, QWidget *parent)
    : QWidget(parent)
    , m_quoteInEditing(false)
    , m_profile(nullptr)
    , m_showProfiles(showProfiles)
    , m_showUpload(showUpload)
    , m_ghnsEditable(false)
    , m_disableUpdate(false)
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    , m_inWorkIcon(BarIcon("view-refresh"))
    , m_okIcon(BarIcon("dialog-ok-apply"))
    , m_failIcon(BarIcon("dialog-cancel"))
#else
    , m_inWorkIcon(QIcon::fromTheme("view-refresh").pixmap(16))
    , m_okIcon(QIcon::fromTheme("dialog-ok-apply").pixmap(16))
    , m_failIcon(QIcon::fromTheme("dialog-cancel").pixmap(16))
#endif
    , m_webPageDialog(nullptr)

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    , m_infoMessage(nullptr)
#endif
    , m_model(nullptr)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    static KComponentData alk(TRANSLATION_DOMAIN);
#endif
    Ui::AlkOnlineQuoteDetailsWidget::setupUi(parent);
    Ui::AlkOnlineQuotesDebugWidget::setupUi(parent);
    Ui::AlkOnlineQuotesProfileDetailsWidget::setupUi(parent);
    Ui::AlkOnlineQuotesProfilesWidget::setupUi(parent);
    Ui::AlkOnlineQuotesWidget::setupUi(parent);

    if (!QString(BUILD_KEY).isEmpty())
        m_buildKey->setText(QString("<small>alkimia version: %1</small>").arg(BUILD_KEY));
    else
        m_buildKey->setText(QString());

#ifdef BUILD_WITH_WEBENGINE
    AlkWebView::setWebInspectorEnabled(true);
#endif
    initLocale();
    m_webView = new AlkWebView;
    m_webView->setWebPage(new AlkWebPage);
#ifdef BUILD_WITH_WEBKIT
    m_webView->setWebInspectorEnabled(true);
#endif
    AlkOnlineQuotesProfileManager::instance().setWebView(m_webView);
    AlkOnlineQuotesProfileManager::instance().setWebPage(m_webView->webPage());

    profilesGroupBox->setVisible(showProfiles);
    profileDetailsBox->setVisible(showProfiles);
    m_showButton->setVisible(!showProfiles && AlkOnlineQuotesProfileManager::instance().webViewEnabled());
    m_ghnsSource->setVisible(false);
    m_urlCheckLabel->setMinimumWidth(m_okIcon.width());

    loadProfiles();

    // TODO move to ui file
    KGuiItem acceptButtonItem(i18nc("Accepts the entered data and stores it", "&Accept"),
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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    KGuiItem::assign(m_acceptButton, acceptButtonItem);
    KGuiItem::assign(m_deleteButton, deleteButtenItem);
    KGuiItem::assign(m_checkButton, checkButtonItem);
    KGuiItem::assign(m_showButton, showButtonItem);
    KGuiItem::assign(m_newButton, newButtenItem);

    m_infoMessage = new KMessageWidget(onlineQuotesGroupBox);
    groupBoxLayout->insertWidget(0, m_infoMessage);
    m_infoMessage->hide();
#else
    m_acceptButton->setGuiItem(acceptButtonItem);
    m_deleteButton->setGuiItem(deleteButtenItem);
    m_checkButton->setGuiItem(checkButtonItem);
    m_showButton->setGuiItem(showButtonItem);
    m_newButton->setGuiItem(newButtenItem);
#endif

    connect(m_newProfile, SIGNAL(clicked()), this, SLOT(slotNewProfile()));
    connect(m_deleteProfile, SIGNAL(clicked()), this, SLOT(slotDeleteProfile()));
    connect(m_profileList, SIGNAL(itemSelectionChanged()), this, SLOT(slotLoadProfile()));

    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(slotLoadQuoteSource()));
    connect(m_acceptButton, SIGNAL(clicked()), this, SLOT(slotAcceptEntry()));
    connect(m_newButton, SIGNAL(clicked()), this, SLOT(slotNewEntry()));
    connect(m_checkButton, SIGNAL(clicked()), this, SLOT(slotCheckEntry()));
    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteEntry()));
    connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(slotDuplicateEntry()));
    connect(m_installButton, SIGNAL(clicked()), this, SLOT(slotInstallEntries()));
    connect(m_uploadButton, SIGNAL(clicked()), this, SLOT(slotUploadEntry()));

    QFontMetrics fm(QApplication::font());
    const int rowHeight = fm.height();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_quoteSourceList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_quoteSourceList->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_quoteSourceList->verticalHeader()->setDefaultSectionSize(rowHeight);
#else
    m_quoteSourceList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_quoteSourceList->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    m_quoteSourceList->verticalHeader()->setDefaultSectionSize(rowHeight);
#endif
    m_quoteSourceList->verticalHeader()->setVisible(false);
    m_quoteSourceList->setShowGrid(false);
    m_quoteSourceList->horizontalHeader()->setVisible(true);
    m_quoteSourceList->setSortingEnabled(true);
    m_quoteSourceList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_quoteSourceList->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(m_quoteSourceList, SIGNAL(clicked(QModelIndex)), this, SLOT(slotLoadQuoteSource(QModelIndex)));

    connect(m_editURL, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editIdentifier, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));

    m_editIdSelector->addItem(i18nc("@item:inlistbox Stock", "Symbol"), AlkOnlineQuoteSource::IdSelector::Symbol);
    m_editIdSelector->addItem(i18nc("@item:inlistbox Stock", "Identification number"), AlkOnlineQuoteSource::IdSelector::IdentificationNumber);
    m_editIdSelector->addItem(i18nc("@item:inlistbox Stock", "Name"), AlkOnlineQuoteSource::IdSelector::Name);
    connect(m_editIdSelector, SIGNAL(currentIndexChanged(int)), this,  SLOT(slotEntryChanged()));

    connect(m_editDate, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editDateFormat, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editDefaultId, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));
    connect(m_editPriceDecimalSeparator, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEntryChanged()));
    connect(m_editPrice, SIGNAL(textChanged(QString)), this, SLOT(slotEntryChanged()));

    m_editDataFormat->addItem(i18nc("@item:inlistbox Stock", "Stripped HTML"), AlkOnlineQuoteSource::DataFormat::StrippedHTML);
    m_editDataFormat->addItem(i18nc("@item:inlistbox Stock", "HTML"), AlkOnlineQuoteSource::DataFormat::HTML);
    m_editDataFormat->addItem(i18nc("@item:inlistbox Stock", "CSV"), AlkOnlineQuoteSource::DataFormat::CSV);
#ifdef BUILD_WITH_WEBKIT
    m_editDataFormat->addItem(i18nc("@item:inlistbox Stock", "CSS"), AlkOnlineQuoteSource::DataFormat::CSS);
#endif
    connect(m_editDataFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEntryChanged()));

    m_editDownloadType->addItem(i18nc("@item:inlistbox Stock", "Default"), AlkOnlineQuoteSource::DownloadType::Default);
    m_editDownloadType->addItem(i18nc("@item:inlistbox Stock", "Javascript"), AlkOnlineQuoteSource::DownloadType::Javascript);
    connect(m_editDownloadType, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEntryChanged()));

    connect(m_ghnsSource, SIGNAL(toggled(bool)), this, SLOT(slotEntryChanged()));
    connect(m_showButton, SIGNAL(clicked()), this, SLOT(slotShowButton()));

    m_uploadButton->setVisible(false);
    m_acceptButton->setEnabled(false);
    slotLoadProfile();
}

AlkOnlineQuotesWidget::Private::~Private()
{
    m_webPageDialog->deleteLater();
    delete m_webView->webPage();
    delete m_webView;
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
    loadQuotesList(true);
}

void AlkOnlineQuotesWidget::Private::loadQuotesList(const bool updateResetList)
{
    if (updateResetList) {
        m_resetList.clear();
    }

    // create or update model stack
    if (!m_model) {
        m_model = new AlkOnlineQuotesModel(m_profile);
        auto proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(m_model);
        m_quoteSourceList->setModel(proxyModel);
    } else {
        m_model->setProfile(m_profile);
    }

    const auto indexes = m_model->match(m_model->index(0, 0), Qt::DisplayRole, m_currentItem.name(), 1, Qt::MatchExactly);
    const auto index = !indexes.isEmpty() ? indexes.at(0) : QModelIndex();

    m_quoteSourceList->setCurrentIndex(index);
    slotLoadQuoteSource(m_quoteSourceList->currentIndex());
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
    m_uploadButton->setEnabled(false);
    const AlkOnlineQuotesProfileList list = AlkOnlineQuotesProfileManager::instance().profiles();
    if (!m_showProfiles) {
        if (list.isEmpty())
            return;
        m_profile = list.first();
        m_installButton->setVisible(m_profile->hasGHNSSupport());
        m_uploadButton->setVisible(m_profile->hasGHNSSupport());
        loadQuotesList(true);
        return;
    }

    for (AlkOnlineQuotesProfile *profile : list) {
        if (m_profileList->currentItem() && m_profileList->currentItem()->text() == profile->name()) {
            m_profile = profile;
            loadQuotesList(true);
            m_installButton->setVisible(profile->hasGHNSSupport());
            m_uploadButton->setVisible(profile->hasGHNSSupport());
            break;
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

void AlkOnlineQuotesWidget::Private::slotLoadQuoteSource(const QModelIndex &index)
{
    Q_UNUSED(index)

    m_quoteInEditing = false;

    m_disableUpdate = true;
    m_editURL->clear();
    m_editIdentifier->clear();
    m_editIdSelector->setCurrentIndex(AlkOnlineQuoteSource::IdSelector::Symbol);
    m_editPriceDecimalSeparator->setCurrentIndex(AlkOnlineQuoteSource::DecimalSeparator::Legacy);
    m_editPrice->clear();
    m_editDate->clear();
    m_editDateFormat->clear();
    m_editDefaultId->clear();

    QString name;
    if (m_quoteSourceList->currentIndex().isValid()) {
        name = m_quoteSourceList->model()->data(m_quoteSourceList->currentIndex(), AlkOnlineQuotesModel::NameRole).toString();
        m_currentItem = AlkOnlineQuoteSource(name, m_profile);
        m_editURL->setText(m_currentItem.url());
        m_editIdentifier->setText(m_currentItem.idRegex());
        m_editIdSelector->setCurrentIndex(m_currentItem.idSelector());
        m_editPriceDecimalSeparator->setCurrentIndex(m_currentItem.priceDecimalSeparator());
        m_editPrice->setText(m_currentItem.priceRegex());
        m_editDataFormat->setCurrentIndex(m_currentItem.dataFormat());
        m_editDate->setText(m_currentItem.dateRegex());
        m_editDateFormat->setText(m_currentItem.dateFormat());
        m_editDefaultId->setText(m_currentItem.defaultId());
        m_editDownloadType->setCurrentIndex(m_currentItem.downloadType());
        m_ghnsSource->setChecked(m_currentItem.isGHNS());
    }

    bool enabled = !name.isEmpty();
    bool isFinanceQuoteSource = (enabled && AlkOnlineQuoteSource::isFinanceQuote(name)) ||
            m_profile->type() == AlkOnlineQuotesProfile::Type::Script;

    if (isFinanceQuoteSource || (m_currentItem.isGHNS() && !m_ghnsEditable))
        enabled = false;

    m_editURL->setEnabled(enabled);
    m_editIdentifier->setEnabled(enabled);
    m_editIdSelector->setEnabled(enabled);
    m_editPriceDecimalSeparator->setEnabled(enabled);
    m_editPrice->setEnabled(enabled);
    m_editDate->setEnabled(enabled);
    m_editDateFormat->setEnabled(enabled);
    m_editDefaultId->setEnabled(enabled);
    m_editDownloadType->setEnabled(enabled);
    m_ghnsSource->setVisible(m_profile && m_profile->hasGHNSSupport());
    m_ghnsSource->setEnabled(m_showUpload && m_profile && m_profile->hasGHNSSupport() && enabled);
    m_uploadButton->setEnabled(m_showUpload && m_profile && m_profile->hasGHNSSupport());
    m_editDataFormat->setEnabled(enabled);

    m_disableUpdate = false;

    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotEntryChanged()
{
    if (!m_disableUpdate)
        updateButtonState();
}

void AlkOnlineQuotesWidget::Private::updateButtonState()
{
    clearIcons();
    bool modified = m_editURL->text() != m_currentItem.url()
                    || m_editIdentifier->text() != m_currentItem.idRegex()
                    || m_editIdSelector->currentIndex() != static_cast<int>(m_currentItem.idSelector())
                    || m_editDataFormat->currentIndex() != static_cast<int>(m_currentItem.dataFormat())
                    || m_editDate->text() != m_currentItem.dateRegex()
                    || m_editDateFormat->text() != m_currentItem.dateFormat()
                    || m_editDefaultId->text() != m_currentItem.defaultId()
                    || m_editPriceDecimalSeparator->currentIndex() != static_cast<int>(m_currentItem.priceDecimalSeparator())
                    || m_editDownloadType->currentIndex() != static_cast<int>(m_currentItem.downloadType())
                    || m_editPrice->text() != m_currentItem.priceRegex()
                    || m_ghnsSource->isChecked() != m_currentItem.isGHNS();

    bool isFinanceQuote = m_currentItem.isFinanceQuote() || m_profile->type() == AlkOnlineQuotesProfile::Type::Script;
    bool hasWriteSupport = (m_profile->type() != AlkOnlineQuotesProfile::Type::None && !isFinanceQuote) || m_profile->hasGHNSSupport();
    bool isRemoteUnpublished = m_currentItem.isGHNS() && m_currentItem.profile()->GHNSFilePath(m_currentItem.name()).isEmpty();
    bool isDefaultSource = m_profile->defaultQuoteSources().contains(m_currentItem.name());
    m_newButton->setEnabled(hasWriteSupport);
    m_cancelButton->setEnabled(modified);
    m_duplicateButton->setEnabled(hasWriteSupport);
    m_deleteButton->setEnabled((!m_currentItem.isReadOnly() && !m_currentItem.isGHNS() && !isDefaultSource) || isRemoteUnpublished);
    m_uploadButton->setEnabled(m_profile->hasGHNSSupport() && m_currentItem.isGHNS() && AlkOnlineQuoteUploadDialog::isSupported());
    m_acceptButton->setEnabled(modified);
    m_checkButton->setEnabled(isFinanceQuote || !modified);
    m_checkSymbol->setEnabled(!m_currentItem.requiresTwoIdentifier());
    m_checkSymbol2->setEnabled(m_currentItem.requiresTwoIdentifier());
    m_editIdSelector->setVisible(m_profile->type() == AlkOnlineQuotesProfile::Type::KMyMoney5);
    m_editIdSelectorLabel->setVisible(m_profile->type() == AlkOnlineQuotesProfile::Type::KMyMoney5);
    if (m_currentItem.requiresTwoIdentifier()) {
        m_checkSymbol->setText("");
        m_checkSymbol2->setText(!m_currentItem.defaultId().isEmpty() ? m_currentItem.defaultId() : "BTC GBP");
    } else {
        m_checkSymbol->setText(!m_currentItem.defaultId().isEmpty() ? m_currentItem.defaultId() : "ORCL");
        m_checkSymbol2->setText("");
    }
    bool isCSVSource = m_currentItem.dataFormat() == AlkOnlineQuoteSource::CSV;
    m_startDateLabel->setVisible(isCSVSource);
    m_endDateLabel->setVisible(isCSVSource);
    m_startDateEdit->setVisible(isCSVSource);
    m_endDateEdit->setVisible(isCSVSource);
}

void AlkOnlineQuotesWidget::Private::slotDeleteEntry()
{
    if (!m_quoteSourceList->currentIndex().isValid())
        return;

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
    m_quoteSourceList->model()->removeRow(m_quoteSourceList->currentIndex().row());
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotDuplicateEntry()
{
    if (!m_quoteSourceList->currentIndex().isValid())
        return;

    AlkOnlineQuoteSource copy(m_currentItem);
    copy.setName(copy.name() + i18n(".copy"));
    copy.setGHNS(false);
    copy.write();
    m_currentItem = copy;
    m_quoteSourceList->model()->insertRow(m_quoteSourceList->currentIndex().row() + 1);
    loadQuotesList();
}

void AlkOnlineQuotesWidget::Private::slotAcceptEntry()
{
    m_currentItem.setUrl(m_editURL->text());
    m_currentItem.setIdRegex(m_editIdentifier->text());
    m_currentItem.setIdSelector(static_cast<AlkOnlineQuoteSource::IdSelector>(m_editIdSelector->currentIndex()));
    m_currentItem.setDataFormat(static_cast<AlkOnlineQuoteSource::DataFormat>(m_editDataFormat->currentIndex()));
    m_currentItem.setDateRegex(m_editDate->text());
    m_currentItem.setDateFormat(m_editDateFormat->text());
    m_currentItem.setDefaultId(m_editDefaultId->text());
    m_currentItem.setPriceDecimalSeparator(static_cast<AlkOnlineQuoteSource::DecimalSeparator>(m_editPriceDecimalSeparator->currentIndex()));
    m_currentItem.setDownloadType(static_cast<AlkOnlineQuoteSource::DownloadType>(m_editDownloadType->currentIndex()));
    m_currentItem.setPriceRegex(m_editPrice->text());
    m_currentItem.setGHNS(m_ghnsSource->isChecked());
    m_currentItem.write();
    m_checkButton->setEnabled(true);
    loadQuotesList();
    updateButtonState();
}

void AlkOnlineQuotesWidget::Private::slotNewEntry()
{
    const bool newEntries = m_profile->quoteSources().contains(i18n("New Quote Source"));
    if (!newEntries) {
        AlkOnlineQuoteSource newSource(i18n("New Quote Source"), m_profile);
        newSource.write();
        m_currentItem = newSource;
        loadQuotesList();
        // TODO select new entry
        //int index = m_profile->quoteSources().indexOf(newSource.name());
        m_quoteSourceList->setCurrentIndex(QModelIndex());

    } else {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (!m_infoMessage->isVisible() && !m_infoMessage->isShowAnimationRunning()) {
            m_infoMessage->resize(width(), m_infoMessage->heightForWidth(width()));
            m_infoMessage->setText(
                i18nc("@info Detail that only one new entry can exist at any time", "<b>New Quote Source</b> already exists."));
            m_infoMessage->setMessageType(KMessageWidget::Information);
            m_infoMessage->animatedShow();
        }
#endif
    }
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
        m_symbolCheckLabel->setPixmap((errors & AlkOnlineQuote::Errors::Symbol) ? m_failIcon : m_okIcon);
        m_priceCheckLabel->setPixmap((errors & AlkOnlineQuote::Errors::Price) ? m_failIcon : m_okIcon);
        if (errors & AlkOnlineQuote::Errors::Date) {
            m_dateCheckLabel->setPixmap(m_failIcon);
        } else {
            if (m_currentItem.dateRegex().isEmpty()) {
                m_dateCheckLabel->setPixmap(m_emptyIcon);
                m_dateFormatCheckLabel->setPixmap(m_emptyIcon);
            } else {
                m_dateCheckLabel->setPixmap(m_okIcon);
                m_dateFormatCheckLabel->setPixmap(
                    (errors & AlkOnlineQuote::Errors::DateFormat) ? m_failIcon : m_okIcon);
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
    connect(&quote, SIGNAL(quotes(QString,QString,AlkDatePriceMap)), this,
            SLOT(slotLogQuotes(QString,QString,AlkDatePriceMap)));
    initIcons();
    if (m_currentItem.dataFormat() == AlkOnlineQuoteSource::CSV) {
        quote.setDateRange(m_startDateEdit->date(), m_endDateEdit->date());
    } else {
        quote.setDateRange(QDate(), QDate());
    }
    if (m_currentItem.requiresTwoIdentifier()) {
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

void AlkOnlineQuotesWidget::Private::slotLogQuotes(const QString &id, const QString &symbol,
                                         const AlkDatePriceMap &prices)
{
    slotLogStatus(QString("<font color=\"green\">%1 %2</font>").arg(id, symbol));

    slotLogStatus(QString("<font color=\"green\">date price</font>"));
    for (auto i = prices.constBegin(), end = prices.constEnd(); i != end; ++i) {
        slotLogStatus(QString("<font color=\"green\">%1 %2</font>")
                      .arg(i.key().toString(Qt::ISODate)).arg(i.value().toDouble()));
    }
}

void AlkOnlineQuotesWidget::Private::slotInstallEntries()
{
    QString configFile = m_profile->hotNewStuffConfigFile();

    AlkNewStuffWidget widget;
    widget.init(configFile);
    if (widget.showInstallDialog()) {
        m_profile->reload();
        loadQuotesList();
    }
}

void AlkOnlineQuotesWidget::Private::slotUploadEntry()
{
    QPointer<AlkOnlineQuoteUploadDialog> dialog = new AlkOnlineQuoteUploadDialog(m_currentItem, false, this);
    dialog->exec();
    delete dialog;
}

void AlkOnlineQuotesWidget::Private::slotShowButton()
{
    if (!m_webPageDialog) {
        m_webPageDialog = new QDialog;
        m_webPageDialog->setWindowTitle(i18n("Online Quote HTML Result Window"));
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(m_webView);
        m_webPageDialog->setLayout(layout);
    }
    m_webPageDialog->show();
}

QString AlkOnlineQuotesWidget::Private::expandedUrl() const
{
    if (m_currentItem.requiresTwoIdentifier()) {
        return m_currentItem.url().arg(m_checkSymbol2->text());
    } else {
        return m_currentItem.url().arg(m_checkSymbol->text());
    }
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

bool AlkOnlineQuotesWidget::GHNSSourceEditable()
{
    return d->m_ghnsEditable;

}

void AlkOnlineQuotesWidget::setGHNSSourceEditable(bool state)
{
    d->m_ghnsEditable = state;
}

#include "alkonlinequoteswidget.moc"
