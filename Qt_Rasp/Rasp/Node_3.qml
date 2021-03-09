import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import CustomControls 1.0

Item {
    id: node1
    property variant stringList
    property int ag: 0
    Rectangle {
        id: rectangle
        width: 800
        height: 420
        color: "#242424"
        anchors.fill: parent
        /*--- temperature ---*/
        RadialBar {
            id: temperature
             x: 10 + 10 +10+10
             y: 10
             width: 150
             height: 150
             fillColor: "#242424"
             backgroundColor: "#242424"
             penStyle: Qt.RoundCap
             dialType: RadialBar.FullDial
             progressColor: "#F7BE24"//"#1dc58f"
             foregroundColor: "#A4A4AB"
             dialWidth: 15
             startAngle: 180
             spanAngle: 70
             minValue: 0
             maxValue: 100
             value: 10
             textFont {
                 family: "Halvetica"
                 italic: false
                 pointSize: 16
             }
             suffixText: "°C"
             textColor: "#FFFFFF"
        }
        /*--- humidity ---*/
        RadialBar {
            id: humidity
             x: 10 + 150 + 10 + 20 +20+20
             y: 10
             width: 150
             height: 150
             fillColor: "#242424"
             backgroundColor: "#242424"
             penStyle: Qt.RoundCap
             dialType: RadialBar.FullDial
             progressColor: "#F7BE24"//"#1dc58f"
             foregroundColor: "#A4A4AB"
             dialWidth: 15
             startAngle: 180
             spanAngle: 70
             minValue: 0
             maxValue: 100
             value: 10
             textFont {
                 family: "Halvetica"
                 italic: false
                 pointSize: 16
             }
             suffixText: "%"
             textColor: "#FFFFFF"
        }
        /*--- Sun intensity ---*/
        RadialBar {
             id: sun
             x: 30+300+30+30+30
             y: 10
             width: 150
             height: 150
             fillColor: "#242424"
             backgroundColor: "#242424"
             penStyle: Qt.RoundCap
             dialType: RadialBar.FullDial
             progressColor: "#F7BE24"//"#1dc58f"
             foregroundColor: "#A4A4AB"
             dialWidth: 15
             startAngle: 180
             spanAngle: 70
             minValue: 0
             maxValue: 65536
             value: 10
             textFont {
                 family: "Halvetica"
                 italic: false
                 pointSize: 16
             }
             suffixText: "Lux"
             textColor: "#FFFFFF"
        }
        /*--- amount of rain ---*/
        RadialBar {
             id: rain
             x: 40+450+40+40+40
             y: 10
             width: 150
             height: 150
             fillColor: "#242424"
             backgroundColor: "#242424"
             penStyle: Qt.RoundCap
             dialType: RadialBar.FullDial
             progressColor: "#F7BE24"//"#1dc58f"
             foregroundColor: "#A4A4AB"
             dialWidth: 15
             startAngle: 180
             spanAngle: 70
             minValue: 0
             maxValue: 100
             value: 10
             textFont {
                 family: "Halvetica"
                 italic: false
                 pointSize: 16
             }
             suffixText: "mm"
             textColor: "#FFFFFF"
        }
        /*--- wind speed ---*/
        RadialBar {
            id: speed
             x: 130
             y: 200
             width: 150
             height: 150
             fillColor: "#242424"
             backgroundColor: "#242424"
             penStyle: Qt.RoundCap
             dialType: RadialBar.FullDial
             progressColor: "#F7BE24"//"#1dc58f"
             foregroundColor: "#A4A4AB"
             dialWidth: 15
             startAngle: 180
             spanAngle: 70
             minValue: 0
             maxValue: 100
             value: 10
             textFont {
                 family: "Halvetica"
                 italic: false
                 pointSize: 16
             }
             suffixText: "rpm"
             textColor: "#FFFFFF"
        }
        /* --- text temperature ---*/
        Text {
            id: txt1
            x: 55
            y: 170
            width: 112
            height: 32
            color: "#ffffff"
            text: qsTr("Temperature")
            rotation: 0
            font.pixelSize: 20
            font.family: "Halvetica"
        }
        /* --- text humidity ---*/
        Text {
            id: txt2
            x: 10 + 150 + 10 + 20 +20 + 20 + 40
            y: 170
            width: 71
            height: 32
            color: "#ffffff"
            text: qsTr("Humidity")
            rotation: 0
            font.pixelSize: 20
            font.family: "Halvetica"
        }
        /*--- Sun intensity ---*/
        Text {
            id: txt3
            x: 440
            y: 170
            width: 112
            height: 32
            color: "#ffffff"
            text: qsTr("Sun intensity")
            font.pixelSize: 20
            rotation: 0
            font.family: "Halvetica"
        }
        /*--- text amount of rain ---*/
        Text {
            id: txt4
            x: 625
            y: 170
            width: 112
            height: 32
            color: "#ffffff"
            text: qsTr("amount of rain")
            font.pixelSize: 20
            rotation: 0
            font.family: "Halvetica"
        }
        /*--- text wind speed ---*/
        Text {
            id: txt5
            x: 160
            y: 360
            width: 112
            height: 32
            color: "#ffffff"
            text: qsTr("wind speed")
            font.pixelSize: 20
            rotation: 0
            font.family: "Halvetica"
        }
        /*--- compass ---*/
        Item {
            Image {
                x: 481
                y: 210
                width: 213
                height: 173
                source: "compass.svg"
            }
            Image {
                x: 578
                y: 247
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

        Text {
            id: txt6
            x: 371
            y: 248
            width: 112
            height: 32
            color: "#ffffff"
            text: qsTr("Direction:")
            font.pixelSize: 20
            rotation: 0
            font.family: "Halvetica"
        }
        Connections
        {
            target: My_config
            onIncreaseOne:
            {
                stringList = ms.split(',')
                if(stringList[0] === "3")
                {
                temperature.value = stringList[1]
                humidity.value = stringList[2]
                sun.value = stringList[3]
                rain.value = stringList[4]
                speed.value = stringList[5]
                ag = stringList[6]
                }
            }
        }

    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:1.25;height:480;width:640}
}
##^##*/
