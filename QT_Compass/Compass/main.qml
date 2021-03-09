import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    property int ag: 30
    Item {
        Image {
            x: 0
            y: 0
            source: "compass.svg"
        }
        Image {
            x: 196
            y: 121
            width: 20
            height: 100
            source: "arrow.svg"
            transform: Rotation {
                origin.x: 10
                origin.y: 50
                angle: ag
                objectName: "rect"
            }
        }

    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.25}
}
##^##*/
