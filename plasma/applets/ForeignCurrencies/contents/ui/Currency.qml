import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.alkimia 1.0

Item {
    id: root;
    property string symbol: "USD EUR";
    property string source: "Alkimia Currency";
    // one hour
    property int timeout: 1000*60*60;
    property double price;
    Layout.fillWidth: true;
    Layout.fillHeight: true;

    Text  {
        id: currencyLabel;
        text: root.symbol;
        font.pixelSize: 18;
        color: theme.textColor;
        height: font.pixelsize*1.1;
        width: parent.width * 0.5;
        horizontalAlignment: Text.AlignLeft;
        verticalAlignment: Text.AlignVCenter;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        }

    Text  {
        id: priceLabel;
        text: i18n("unknown");
        font.pixelSize: 18;
        color: theme.textColor;
        height: font.pixelsize*1.1;
        width: parent.width * 0.5;
        horizontalAlignment: Text.AlignRight;
        verticalAlignment: Text.AlignVCenter;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.left: currencyLabel.right;
        }

    AlkOnlineQuote {
        id: quote
        onError: {
            console.log(s)
        }
        onStatus: {
            console.log(s)
        }
        onQuote: {
            //root.date = date
            priceLabel.text = price
        }
    }

    Timer {
        id: timer;
        interval: 1000;
        running: true;
        repeat: true;
        onTriggered: {
            console.log("=== timer ===");
            if (root.symbol == "") {
                timer.interval = 10000;
                return;
            }
            quote.launch(root.symbol, "", root.source)
            timer.interval = root.timeout
        }
    }
}

