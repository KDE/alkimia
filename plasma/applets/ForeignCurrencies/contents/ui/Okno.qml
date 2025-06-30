import QtQuick 2.0
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore

ColumnLayout {
    Layout.minimumWidth:    PlasmaCore.Units.gridUnit * 10
    Layout.minimumHeight:   PlasmaCore.Units.gridUnit *  5
    Layout.preferredWidth:  PlasmaCore.Units.gridUnit * 20
    Layout.preferredHeight: PlasmaCore.Units.gridUnit * 50

    id: root
    anchors.fill: parent;

    Component.onCompleted: {
        createLines();
        }

    Connections {
        target: plasmoid.configuration;
        onCurrenciesChanged: { createLines(); }
        }

    function createLines() {
        for (var i=root.children.length-1; i >= 0; i--) {
            root.children[i].destroy();
            }
        var currencies = JSON.parse(plasmoid.configuration.currencies)
        currencies.forEach(function (line) {
            var component = Qt.createComponent("Currency.qml");
            var chart = component.createObject(root, { symbol: line.symbol });
            })

        }
}

