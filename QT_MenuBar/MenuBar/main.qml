import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    property alias page: Page1
    Button {
        id: button
        x: 0
        y: 0
        text: qsTr("1")
    }

    Button {
        id: button1
        x: 100
        y: 0
        text: qsTr("2")
    }

    Button {
        id: button2
        x: 200
        y: 0
        text: qsTr("3")
    }

    Rectangle {
        id: rectangle
        x: 0
        y: 40
        width: 640
        height: 440
        color: "#242424"
        SwipeView {
            id: view

            currentIndex: 1
            anchors.fill: parent

            Page1 {
                id: firstPage
            }
            Page2 {
                id: secondPage
            }
        }
        /*---PageIndicator---*/
        PageIndicator {
            id: indicator

            count: view.count
            currentIndex: view.currentIndex

            anchors.bottom: view.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            delegate: Rectangle {
                    implicitWidth: 8
                    implicitHeight: 8

                    radius: width / 2
                    color: "#21be2b"

                    opacity: index === indicator.currentIndex ? 0.95 : pressed ? 0.7 : 0.45

                    Behavior on opacity {
                        OpacityAnimator {
                            duration: 100
                        }
                    }
                }
        }
    }
}
