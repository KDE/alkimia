import QtQuick 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

Item {

    property string cfg_currencies

    ListModel {
        id: currenciesModel
        }

    Component.onCompleted: {
        var currencies = JSON.parse(plasmoid.configuration.currencies)
        currencies.forEach(function (line) {
            currenciesModel.append({
                symbol: line.symbol
                })
            })
        }


    function currenciesModelChanged() {
        var newCurrenciesArray = []
        for (var i = 0; i < currenciesModel.count; i++) {
            newCurrenciesArray.push({
                symbol: currenciesModel.get(i).symbol
            })
        }
        cfg_currencies = JSON.stringify(newCurrenciesArray)
    }


    Dialog {
        id: addCurrencyDialog
        title: i18n("Add Currency Pair")

        width: 300

        standardButtons: StandardButton.Ok | StandardButton.Cancel

        onAccepted: {
            if (symbolField.text !== "") { 
                console.log("symbol: " + symbolField.text);
                currenciesModel.append({ symbol: symbolField.text });
                currenciesModelChanged()
                close()
                }
            }

        TextField {
            id: symbolField
            placeholderText: i18n("Paste currency pair symbol here")
            width: parent.width
            }

        }


    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        Label {
            text: i18n("Currency Pairs")
            font.bold: true
            Layout.alignment: Qt.AlignLeft
            }

        Item { }

        TableView {
            id: currenciesTable
            width: parent.width

            TableViewColumn {
                id: symbolIdCol
                role: 'symbolId'
                title: i18n("Symbol")
                // width: parent.width * 0.6

                delegate: Label {
                    text: styleData.value
                    elide: Text.ElideRight
                    anchors.left: parent ? parent.left : undefined
                    anchors.leftMargin: 5
                    anchors.right: parent ? parent.right : undefined
                    anchors.rightMargin: 5
                }
            }


            TableViewColumn {
                title: i18n("Action")
                // width: parent.width * 0.2

                delegate: Item {

                    GridLayout {
                        height: parent.height
                        columns: 3
                        rowSpacing: 0

                        Button {
                            iconName: 'go-up'
                            Layout.fillHeight: true
                            onClicked: {
                                currenciesModel.move(styleData.row, styleData.row - 1, 1)
                                currenciesModelChanged()
                            }
                            enabled: styleData.row > 0
                        }

                        Button {
                            iconName: 'go-down'
                            Layout.fillHeight: true
                            onClicked: {
                                currenciesModel.move(styleData.row, styleData.row + 1, 1)
                                currenciesModelChanged()
                            }
                            enabled: styleData.row < currenciesModel.count - 1
                        }

                        Button {
                            iconName: 'list-remove'
                            Layout.fillHeight: true
                            onClicked: {
                                currenciesModel.remove(styleData.row)
                                currenciesModelChanged()
                            }
                        }
                    }
                }

            }
            model: currenciesModel
            Layout.preferredHeight: 150
            Layout.preferredWidth: parent.width
            Layout.columnSpan: 2
        }

        Row {
            Layout.columnSpan: 2

            Button {
                iconName: 'list-add'
                text: i18n("Add Currency Pair")
                onClicked: {
                    addCurrencyDialog.open();
                    symbolField.text = ''
                    symbolField.focus = true
                }
            }

        }

    }

}
