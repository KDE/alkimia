import QtQuick 2.0
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: root;
    property string symbol: "USD_EUR";
    Layout.fillWidth: true;
    Layout.fillHeight: true;

    MouseArea {
        anchors.fill: root;
        onClicked: {
            console.log("clicked");
            var currencyPair = root.symbol.split('_')
            var suffix = currencyPair[0] + currencyPair[1] + "=X"
            Qt.openUrlExternally("https://finance.yahoo.com/quote/" + suffix)
            }
        }

    Text  {
        id: currency;
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
        id: price;
        text: "unknown";
        font.pixelSize: 18;
        color: theme.textColor;
        height: font.pixelsize*1.1;
        width: parent.width * 0.5;
        horizontalAlignment: Text.AlignRight;
        verticalAlignment: Text.AlignVCenter;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.left: currency.right;
        }

    Timer {
        id: timer;
        interval: 50;
        running: true;
        repeat: true;
        onTriggered: {
            console.log("=== timer ===");
            if (symbol == "") {
                timer.interval = 10000;
                return;
                }
            var date = new Date();
            var rq = new XMLHttpRequest();
      
            // http://free.currencyconverterapi.com/api/v5/convert?q=EUR_USD&compact=y
            var url = "http://free.currencyconverterapi.com/api/v5/convert?q="
            url += symbol
            url += "&compact=y"
            console.log(url)
            
            rq.onreadystatechange = function() {
                if (rq.readyState == XMLHttpRequest.DONE && rq.status == 200) {
                    console.log("Http response:", rq.responseText)
                    var jsonObject = eval('(' + rq.responseText + ')');
                    price.text = jsonObject[symbol]["val"];
                }
            }
           
            rq.open("GET", url, true);
            rq.send();
            timer.interval = 900000
            }
        }

}

