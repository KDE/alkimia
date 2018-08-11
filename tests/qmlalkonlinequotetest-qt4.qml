import QtQuick 1.0
import org.kde.alkimia 1.0

Item {
    width: 500
    height: 300

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

    Text {
        id: symbolLabel
        y: 50
        x: 0
        text: "Enter Symbol:"
    }
    TextInput {
        y: 50
        x: 100
        id: symbol
        text: "CAD EUR"
        //placeholderText: qsTr("Symbol")
    }

    Text {
        id: quoteLabel
        y: 80
        x: 0
        text: "Enter Quote:"
    }
    TextInput {
        y: 80
        x: 100
        id: source
        text: "Alkimia Currency"
        //placeholderText: qsTr("Source")
    }

    Rectangle {
        width: 200
        height: 30
        y: 122
        x: 100
        color: "lightgray"
        MouseArea {
            id: mouseArea;
            anchors.fill: parent; 

            onClicked: {
                dateLabel.text = ""
                priceLabel.text = ""
                errorLabel.text = ""
                statusLabel.text = ""
                quote.launch(symbol.text, "", source.text)
            }
        }
    }

    Text {
        id: fetchLabel
        y: 128
        x: 170
        text: "Fetch"
    }

    Text {
        id: statusLabel
        y: 150
        x: 100
    }

    Text {
        id: priceLabel
        y: 180
        x: 100
    }

    Text {
        id: dateLabel
        y: 210
        x: 100
    }

    Text {
        id: errorLabel
        y: 240
        x: 100
    }
}
