#ifndef ONLINEQUOTE_HEADER
#define ONLINEQUOTE_HEADER

#include <KIcon>
// Here we avoid loading the header multiple times
#include <Plasma/Applet>
#include <Plasma/Svg>
 
class QSizeF;
 
// Define our plasma Applet
class PlasmaOnlineQuote : public Plasma::Applet
{
    Q_OBJECT
    public:
        // Basic Create/Destroy
        PlasmaOnlineQuote(QObject *parent, const QVariantList &args);
        ~PlasmaOnlineQuote();
 
        // The paintInterface procedure paints the applet to screen
        void paintInterface(QPainter *p,
                const QStyleOptionGraphicsItem *option,
                const QRect& contentsRect);
	void init();

    private:
        Plasma::Svg m_svg;
        KIcon m_icon;
};

#endif
