#include "plasma-onlinequote.h"

#include "alkonlinequote.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"
#include "ui_configwidget.h"

#include <QComboBox>
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTimer>
#include <QtDebug>

#include <plasma/svg.h>
#include <plasma/theme.h>

#include <KConfigDialog>


class MyWidget : public QWidget, public Ui::ConfigWidget
{
public:
    MyWidget()
    {
        setupUi((QWidget *)this);
    }
};

PlasmaOnlineQuote::PlasmaOnlineQuote(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
    , m_svg(this)
    , m_icon("preferences-system-network")
    , m_widget(0)
    , m_price(0)
    , m_profile(nullptr)
{
    setHasConfigurationInterface(true);
    m_svg.setImagePath("widgets/background");
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    resize(200, 200);
#endif
    AlkOnlineQuotesProfileManager &manager = AlkOnlineQuotesProfileManager::instance();
    // manager is shared between plasmoids
    if(AlkOnlineQuotesProfileManager::instance().profiles().size() == 0) {
        manager.addProfile(new AlkOnlineQuotesProfile("alkimia4", AlkOnlineQuotesProfile::Type::Alkimia4));
        manager.addProfile(new AlkOnlineQuotesProfile("alkimia5", AlkOnlineQuotesProfile::Type::Alkimia5));
        manager.addProfile(new AlkOnlineQuotesProfile("kmymoney4", AlkOnlineQuotesProfile::Type::KMyMoney4));
        manager.addProfile(new AlkOnlineQuotesProfile("kmymoney5", AlkOnlineQuotesProfile::Type::KMyMoney5));
    }
}

PlasmaOnlineQuote::~PlasmaOnlineQuote()
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else
#endif
    {
        // Save settings
        config().sync();
    }
}

void PlasmaOnlineQuote::init()
{
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    // A small demonstration of the setFailedToLaunch function
    if (m_icon.isNull()) {
        setFailedToLaunch(true, "No world to say hello");
    }
#endif
    QString currentProfile = config().readEntry("profile");
    qDebug() << "reading current profile" << currentProfile;
    if (currentProfile.isEmpty())
        currentProfile = AlkOnlineQuotesProfileManager::instance().profiles().first()->name();
    qDebug() << "setup current profile" << currentProfile;
    m_profile = AlkOnlineQuotesProfileManager::instance().profile(currentProfile);
    QTimer::singleShot(100, this, SLOT(slotFetchQuote()));
}

void PlasmaOnlineQuote::configChanged()
{
    qDebug() << "configChanged()";
}

void PlasmaOnlineQuote::createConfigurationInterface(KConfigDialog *parent)
{
    m_widget = new MyWidget;
    QStringList profiles = AlkOnlineQuotesProfileManager::instance().profileNames();
    m_widget->m_profile->addItems(profiles);
    QString currentProfile = m_profile->name();
    int index = profiles.indexOf(currentProfile);
    m_widget->m_profile->setCurrentIndex(index);
    connect(m_widget->m_profile, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotProfileChanged(QString)));

    QStringList sources = m_profile->quoteSources();
    m_widget->m_onlineQuote->addItems(sources);
    index = sources.indexOf(config().readEntry("onlinequote"));
    m_widget->m_onlineQuote->setCurrentIndex(index);

    m_widget->m_symbol->setText(config().readEntry("symbol"));
    m_widget->m_interval->setValue(config().readEntry("interval", 60));
    parent->addPage(dynamic_cast<QWidget*>(m_widget), "Online Source");
    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
}

void PlasmaOnlineQuote::slotProfileChanged(const QString &name)
{
    AlkOnlineQuotesProfile *profile = AlkOnlineQuotesProfileManager::instance().profile(name);
    if (!profile) {
        qWarning() << "profile" << name << "not present";
        return;
    }
    m_profile = profile;
    QStringList sources = m_profile->quoteSources();
    m_widget->m_onlineQuote->clear();
    m_widget->m_onlineQuote->addItems(sources);
    int index = sources.indexOf(config().readEntry("onlinequote"));
    m_widget->m_onlineQuote->setCurrentIndex(index);
}

void PlasmaOnlineQuote::slotConfigAccepted()
{
    config().writeEntry("profile", m_widget->m_profile->currentText());
    config().writeEntry("onlinequote", m_widget->m_onlineQuote->currentText());
    config().writeEntry("symbol", m_widget->m_symbol->text());
    config().writeEntry("interval", m_widget->m_interval->value());
    config().sync();
    emit configNeedsSaving();
    slotFetchQuote();
    qDebug() << "configAccepted()";
}

void PlasmaOnlineQuote::slotFetchQuote()
{
    if (config().readEntry("symbol").isEmpty() || config().readEntry("interval").toInt() == 0) {
        qDebug() << __FUNCTION__ << "no configuration found";
        return;
    }
    AlkOnlineQuote quote(m_profile);
    connect(&quote, SIGNAL(status(QString)), this, SLOT(slotLogStatus(QString)));
    connect(&quote, SIGNAL(error(QString)), this, SLOT(slotLogError(QString)));
    connect(&quote, SIGNAL(failed(QString,QString)), this, SLOT(slotLogFailed(QString,QString)));
    connect(&quote, SIGNAL(quote(QString,QString,QDate,double)), this, SLOT(slotReceivedQuote(QString,QString,QDate,double)));
    quote.launch(config().readEntry("symbol"), "", config().readEntry("onlinequote"));
    int interval = config().readEntry("interval").toInt()*1000;
    qDebug() << "setting timer to " << interval << "ms";
    QTimer::singleShot(interval, this, SLOT(slotFetchQuote()));
}

void PlasmaOnlineQuote::slotLogStatus(const QString &s)
{
    qDebug() << s;
}

void PlasmaOnlineQuote::slotLogError(const QString &s)
{
    slotLogStatus("Error:" + s);
}

void PlasmaOnlineQuote::slotLogFailed(const QString &id, const QString &symbol)
{
    slotLogStatus(QString("Failed: %1 %2").arg(id, symbol));
}

void PlasmaOnlineQuote::slotReceivedQuote(const QString &id, const QString &symbol, const QDate &date, const double &price)
{
    Q_UNUSED(id)
    Q_UNUSED(symbol)
    qDebug() << "got quote" << date << price;
    m_date = date;
    m_price = price;
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    update();
#else
#warning how to update ui ?
#endif
}

void PlasmaOnlineQuote::paintInterface(QPainter *p,
        const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    Q_UNUSED(option)
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    p->setRenderHint(QPainter::Antialiasing);
 
    // Now we draw the applet, starting with our svg
    m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
    m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());
 
    // We place the icon and text
    p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(),(int)contentsRect.width()-14));
    p->save();
    p->setPen(Qt::white);
    if (true) {
        qDebug() << "drawing" << config().readEntry("onlinequote") << "price" << m_price;
        p->drawText(contentsRect,
                    Qt::AlignVCenter | Qt::AlignHCenter,
                    config().readEntry("onlinequote"));
        p->drawText(contentsRect,
                    Qt::AlignBottom | Qt::AlignHCenter,
                    config().readEntry("symbol") + " " + QString::number(m_price,8,4));

    } else {
        p->drawText(contentsRect,
                Qt::AlignBottom | Qt::AlignHCenter,
                "Hello Plasmoid!");
    }
    p->restore();
}

// This is the command that links your applet to the .desktop file
K_EXPORT_PLASMA_APPLET(onlinequote, PlasmaOnlineQuote)
