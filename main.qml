import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtGraphicalEffects 1.15

import App 1.0

Window {
    width: 800
    height: 600
    visible: true
    title: qsTr("QtWs 2022")
    flags: Qt.Dialog

    // We need to instantiate a single BackgroundRenderer to every QML window
    SceneProvider {}

    SeparateWindow {
        id: separateWindow
    }

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            Layout.preferredWidth: 320
            Layout.fillHeight: true

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "Watermelon classic"
            }

            Scene {
                Layout.preferredWidth: 320
                Layout.preferredHeight: 240
            }

            Label {
                Layout.alignment: Qt.AlignHCenter
                text: "QtGraphical watermelon"
            }

            Scene {
                Layout.preferredWidth: 320
                Layout.preferredHeight: 240

                layer.enabled: true
                layer.effect: GaussianBlur {
                    radius: 8
                    samples: 16
                }

            }
        }

        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent

                Button {
                    text: "Open separate window"
                    onClicked: separateWindow.showNormal()
                }

                Button {
                    text: animation.running ? "Stop animation" : "Start animation"
                    onClicked: animation.running ? animation.stop() : animation.start()

                    ParallelAnimation {
                        id: animation

                        loops: Animation.Infinite

                        SequentialAnimation {
                            NumberAnimation {
                                target: pitchSlider
                                property: "value"
                                easing.type: Easing.InCubic
                                from: 0
                                to: 360
                                duration: 5000
                            }
                            NumberAnimation {
                                target: pitchSlider
                                property: "value"
                                easing.type: Easing.InCubic
                                from: 360
                                to: 0
                                duration: 5000
                            }
                        }
                        SequentialAnimation {
                            NumberAnimation {
                                target: yawSlider
                                property: "value"
                                easing.type: Easing.InOutQuad
                                from: 0
                                to: 360
                                duration: 3000
                            }
                            NumberAnimation {
                                target: yawSlider
                                property: "value"
                                easing.type: Easing.InOutQuad
                                from: 360
                                to: 0
                                duration: 3000
                            }
                        }

                        SequentialAnimation {
                            NumberAnimation {
                                target: rollSlider
                                property: "value"
                                easing.type: Easing.InOutBack
                                from: 0
                                to: 360
                                duration: 6000
                            }
                            NumberAnimation {
                                target: rollSlider
                                property: "value"
                                easing.type: Easing.InOutBack
                                from: 360
                                to: 0
                                duration: 6000
                            }
                        }
                    }
                }

                Label {
                    text: "Pitch"
                }

                Slider {
                    id: pitchSlider

                    from: 0
                    to: 360

                    onValueChanged: resourceService.pitch = value
                }

                Label {
                    text: "Yaw"
                }

                Slider {
                    id: yawSlider

                    from: 0
                    to: 360

                    onValueChanged: resourceService.yaw = value
                }

                Label {
                    text: "Roll"
                }

                Slider {
                    id: rollSlider

                    from: 0
                    to: 360

                    onValueChanged: resourceService.roll = value
                }
            }
        }
    }
}
