import QtQuick 2.6
import QtQuick.Controls 2.0
import org.kde.AlkOnlineQuote 1.0

ApplicationWindow {
    id: root
    width: 500
    height: 300
    visible: true

    AlkOnlineQuote {
        id: quote
        onError: {
            console.log(s)
            errorLabel.text = s
        }
        onStatus: {
            console.log(s)
            statusLabel.text = s
        }
        onQuote: {
            statusLabel.text = ""
            dateLabel.text = "Date: " + date
            priceLabel.text = "Price: " + price
        }
    }

    TextField {
        y: 50
        x: 50
        id: symbol
        text: "CAD EUR"
        placeholderText: qsTr("Symbol")
    }

    TextField {
        y: 80
        x: 50
        id: source
        text: "Alkimia Currency"
        placeholderText: qsTr("Source")
    }

    Button {
        width: 200
        height: 30
        y: 122
        x: 50
        text: "fetch"

        onClicked: {
            dateLabel.text = ""
            priceLabel.text = ""
            errorLabel.text = ""
            statusLabel.text = ""
            quote.launch(symbol.text, "", source.text)
        }
    }

    Label {
        id: statusLabel
        y: 150
        x: 50
    }

    Label {
        id: priceLabel
        y: 180
        x: 50
    }

    Label {
        id: dateLabel
        y: 210
        x: 50
    }

    Label {
        id: errorLabel
        y: 240
        x: 50
    }
}
