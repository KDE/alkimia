import QtQuick 2.2
import org.kde.plasma.configuration 2.0

ConfigModel {
    ConfigCategory {
         name: i18n('General')
         icon: Qt.resolvedUrl('../icons/currency.svg').replace('file://', '')
         source: 'config/ConfigGeneral.qml'
    }
}

