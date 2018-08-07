#ifndef PLASMA_APPLET_ONLINEQUOTE_HEADER
#define PLASMA_APPLET_ONLINEQUOTE_HEADER

#include <KIcon>
// Here we avoid loading the header multiple times
#include <Plasma/Applet>
#include <Plasma/Svg>
 
class QSizeF;

class MyWidget;

// Define our plasma Applet
class PlasmaOnlineQuote : public Plasma::Applet
{
    Q_OBJECT
public:
    // Basic Create/Destroy
    PlasmaOnlineQuote(QObject *parent, const QVariantList &args);
    ~PlasmaOnlineQuote();

protected slots:
    void slotConfigAccepted();
    void slotFetchQuote();
    void slotReceivedQuote(const QString &id, const QString &symbol, const QDate &data, const double &price);
    void slotLogStatus(const QString &s);
    void slotLogError(const QString &s);
    void slotLogFailed(const QString &id, const QString &symbol);
    void slotProfileChanged(const QString &name);
protected:
    // The paintInterface procedure paints the applet to screen
    void paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect& contentsRect);
    void init();
    void configChanged();
    void createConfigurationInterface(KConfigDialog *parent);
    Plasma::Svg m_svg;
    KIcon m_icon;
    MyWidget *m_widget;
    QDate m_date;
    double m_price;
};

#endif
