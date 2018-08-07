#include "plasma-onlinequote.h"

#include "alkonlinequote.h"
#include "alkonlinequotesprofilemanager.h"
#include "ui_configwidget.h"

#include <QComboBox>
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QTimer>

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
    : Plasma::Applet(parent, args),
    m_svg(this),
    m_icon("document"),
    m_widget(0),
    m_price(0)
{
    setHasConfigurationInterface(true);
    m_svg.setImagePath("widgets/background");
    // this will get us the standard applet background, for free!
    setBackgroundHints(DefaultBackground);
    resize(200, 200);
}

PlasmaOnlineQuote::~PlasmaOnlineQuote()
{
    if (hasFailedToLaunch()) {
        // Do some cleanup here
    } else {
        // Save settings
        config().sync();
    }
}

void PlasmaOnlineQuote::init()
{
    // A small demonstration of the setFailedToLaunch function
    if (m_icon.isNull()) {
        setFailedToLaunch(true, "No world to say hello");
    }
    AlkOnlineQuotesProfileManager &manager = AlkOnlineQuotesProfileManager::instance();
    manager.addProfile(new AlkOnlineQuotesProfile("kmymoney", AlkOnlineQuotesProfile::Type::KMyMoney));
    AlkOnlineQuoteSource::setProfile(AlkOnlineQuotesProfileManager::instance().profiles().first());
    QTimer::singleShot(100, this, SLOT(slotFetchQuote()));
}

void PlasmaOnlineQuote::configChanged()
{
    qDebug() << "configChanged()";
}

void PlasmaOnlineQuote::createConfigurationInterface(KConfigDialog *parent)
{
    m_widget = new MyWidget;
    QStringList sources = AlkOnlineQuote::quoteSources();
    m_widget->m_onlineQuote->clear();
    m_widget->m_onlineQuote->addItems(sources);
    int index = sources.indexOf(config().readEntry("onlinequote"));
    m_widget->m_onlineQuote->setCurrentIndex(index);
    m_widget->m_symbol->setText(config().readEntry("symbol"));
    m_widget->m_interval->setValue(config().readEntry("interval", 60));
    parent->addPage(dynamic_cast<QWidget*>(m_widget), "Online Source");
    connect(parent, SIGNAL(applyClicked()), this, SLOT(slotConfigAccepted()));
    connect(parent, SIGNAL(okClicked()), this, SLOT(slotConfigAccepted()));
}

void PlasmaOnlineQuote::slotConfigAccepted()
{
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
    AlkOnlineQuote quote;
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
    slotLogStatus(QString("<font color=\"red\"><b>") + s + QString("</b></font>"));
}

void PlasmaOnlineQuote::slotLogFailed(const QString &id, const QString &symbol)
{
    slotLogStatus(QString("%1 %2").arg(id, symbol));
}

void PlasmaOnlineQuote::slotReceivedQuote(const QString &id, const QString &symbol, const QDate &date, const double &price)
{
    Q_UNUSED(id)
    Q_UNUSED(symbol)
    qDebug() << "got quote" << date << price;
    m_date = date;
    m_price = price;
    update();
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
        qDebug() << "drawing" << m_price;
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
 
#include "plasma-onlinequote.moc"
