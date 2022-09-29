import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import App 1.0

Window {
    id: root

    width: 655
    height: 490

    SceneProvider {

    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: 5
        columns: 2
        rows: 2

        Scene {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Scene {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
        Scene {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Scene {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
