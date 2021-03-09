import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    width: 800
    height: 480
    color: "#242424"
    title: qsTr("Hello World")

    property bool setting_lora: false
    property int location: 0

    /*--- Sensor ---*/
    Rectangle {
        id: rectangle
        x: -393
        y: 49
        width: 372
        height: 200
        color: "#00c6ff"
        MouseArea
        {
            anchors.rightMargin: 0
            anchors.fill: parent
            onClicked:
            {
                if( setting_lora === false)
                {
                console.info("OKE")
                reg_sensor_false.start()
                reg_time_false.start()
                reg_chart_false.start()
                reg_setting_false.start()
                reg_line_false.start()
                txt_lora_false.start()
                image_signal_false.start()
                ani_page_sensor_true.start()
                }
            }
        }
        AnimatedImage {
            id: animatedImage
            x: 8
            y: 23
            width: 100
            height: 100
            source: "../Image/sensor.png"
        }
        NumberAnimation on x
        {
            id: reg_sensor_true
            running: false
            to: 20
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on x
        {
            id: reg_sensor_false
            running: false
            to: -393
            duration: 100
            //loops: Animation.Infinite
        }
        Text {
            id: txt_sensor_config

            x: 114
            y: 61
            width: 215
            height: 47
            color: "#ffffff"
            text: qsTr("SENSOR CONFIG")
            font.pixelSize: 30
            font.weight: Font.Normal
            font.family: "Halvetica"
            font.italic: false
            font.bold: false
            styleColor: "#ffffff"
            font.strikeout: false
            style: Text.Sunken
            font.capitalization: Font.MixedCase
        }
    }
    /*--- Time ---*/
    Rectangle {
        id: rectangle1
        x: -393
        y: 261
        width: 372
        height: 200
        color: "#f39c12"

        AnimatedImage {
            id: animatedImage2
            x: 8
            y: 8
            width: 100
            height: 100
            source: "../Image/time.png"
        }

        Text {
            id: txt_time_hour
            x: 125
            y: 43
            width: 215
            height: 47
            color: "#ffffff"
            text: qsTr("--------")
            style: Text.Sunken
            font.weight: Font.Light
            font.capitalization: Font.MixedCase
            font.strikeout: false
            font.italic: false
            font.bold: false
            font.pixelSize: 40
            font.family: "Halvetica"
            styleColor: "#ffffff"
        }

        Text {
            id: txt_time_date
            x: 8
            y: 131
            width: 215
            height: 47
            color: "#ffffff"
            text: qsTr("----------")
            font.pixelSize: 35
            font.weight: Font.Light
            font.family: "Arial"
            font.italic: false
            font.bold: false
            styleColor: "#ffffff"
            font.strikeout: false
            style: Text.Sunken
            font.capitalization: Font.MixedCase
        }
        NumberAnimation on x
        {
            id: reg_time_true
            running: false
            to: 20
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on x
        {
            id: reg_time_false
            running: false
            to: -394
            duration: 100
            //loops: Animation.Infinite
        }
    }
    /*--- Chart ---*/
    Rectangle {
        id: rectangle2
        x: 822
        y: 49
        width: 372
        height: 200
        color: "#90df30"
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                if( setting_lora === false)
                {
                console.info("OKE")
                reg_sensor_false.start()
                reg_time_false.start()
                reg_chart_false.start()
                reg_setting_false.start()
                reg_line_false.start()
                txt_lora_false.start()
                image_signal_false.start()

                ani_page_chart_true.start()
                }
            }
        }
        AnimatedImage {
            id: animatedImage1
            x: 8
            y: 8
            width: 100
            height: 100
            source: "../Image/chart.png"
        }
        NumberAnimation on x
        {
            id: reg_chart_true
            running: false
            to: 408
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on x
        {
            id: reg_chart_false
            running: false
            to: 822
            duration: 100
            //loops: Animation.Infinite
        }
    }
    /*--- Seting ---*/
    Rectangle {
        id: rectangle3
        x: 822
        y: 261
        width: 372
        height: 200
        color: "#7f5daa"
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                if( setting_lora === false)
                {
                console.info("OKE")
                reg_sensor_false.start()
                reg_time_false.start()
                reg_chart_false.start()
                reg_setting_false.start()
                reg_line_false.start()
                txt_lora_false.start()
                image_signal_false.start()

                ani_page_setting_true.start()
                }
            }
        }
        AnimatedImage {
            id: animatedImage3
            x: 8
            y: 8
            width: 100
            height: 100
            source: "../Image/setting.png"
        }
        NumberAnimation on x
        {
            id: reg_setting_true
            running: false
            to: 408
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on x
        {
            id: reg_setting_false
            running: false
            to: 822
            duration: 100
            //loops: Animation.Infinite
        }
    }
    /*--- TIMER for time ---*/
    Timer {
            interval: 1000; running: true; repeat: true
            onTriggered:
            {
                txt_time_hour.text = Qt.formatDateTime(new Date(), "HH:mm:ss a")
                txt_time_date.text = Qt.formatDateTime(new Date(), "ddd MMMM d yyyy")
            }

    }
    /* --- White line ---*/
    Rectangle
    {
        id: rectangle4
        x: 37
        y: -22
        width: 563
        height: 3
        color: "#ffffff"
        NumberAnimation on y
        {
            id: reg_line_true
            running: false
            to: 20
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: reg_line_false
            running: false
            to: -18
            duration: 100
            //loops: Animation.Infinite
        }
    }
    /*--- Text lora gateway ---*/
    Text {
        id: element
        x: 612
        y: -32
        color: "#ffffff"
        text: qsTr("Lora gateway")
        font.bold: true
        font.family: "Arial"
        font.pixelSize: 20
        NumberAnimation on y
        {
            id: txt_lora_true
            running: false
            to: 10
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: txt_lora_false
            running: false
            to: -28
            duration: 100
            //loops: Animation.Infinite
        }
    }
    /*--- Delay button ---*/
    DelayButton {
        id: btn_delay
        x: 322
        y: 161
        width: 156
        height: 159
        visible: true
        delay: 500
        onActivated:
        {
           console.debug("Activated!")
            btn_delay.visible = false
            reg_sensor_true.start()
            reg_time_true.start()
            reg_chart_true.start()
            reg_setting_true.start()
            reg_line_true.start()
            txt_lora_true.start()
            image_signal_true.start()

            My_config.connet()
            /*
            var component = Qt.createComponent("form_2.qml")
            var window    = component.createObject(root)
            root.visible = false
            window.show()
            */
        }
        contentItem: Text {
            font.bold: true
            font.pointSize: 12
            opacity: enabled ? 1.0 : 0.3
            color: "white"
            text: "START"
            layer.enabled: false
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        background: Rectangle {
            implicitWidth: 100
            implicitHeight: 100
            opacity: enabled ? 1 : 0.3
            color: btn_delay.down ? "#17a81a" : "#21be2b"
            radius: size / 2

            readonly property real size: Math.min(btn_delay.width, btn_delay.height)
            width: size
            height: size
            anchors.centerIn: parent

            Canvas {
                id: canvas
                anchors.fill: parent

                Connections {
                    target: btn_delay
                    function onProgressChanged() { canvas.requestPaint(); }
                }

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    ctx.strokeStyle = "white"
                    ctx.lineWidth = parent.size / 20
                    ctx.beginPath()
                    var startAngle = Math.PI / 5 * 3
                    var endAngle = startAngle + btn_delay.progress * Math.PI / 5 * 9
                    ctx.arc(width / 2, height / 2, width / 2 - ctx.lineWidth / 2 - 2, startAngle, endAngle)
                    ctx.stroke()
                }
            }
        }
    }
    /*-- page sensor ---*/
    Rectangle {
        id: rectangle5
        x: 0
        y: -530
        width: 800
        height: 480
        color: "#242424"
        border.color: "#242424"
        /*--- page ---*/
        Page
        {
            anchors.fill: parent
            SwipeView {
                id: swipeView
                anchors.fill: parent
                currentIndex: tabBar.currentIndex

                Node_1 {
                    id: node_1
                }

                Node_2 {
                    id: node_2
                }
                Node_3 {
                    id: node_3
                }
                Node_4 {
                    id: node_4
                }
            }

            footer: TabBar {
                id: tabBar
                currentIndex: swipeView.currentIndex
                TabButton {
                    text: qsTr("Node 1")
                }
                TabButton {
                    text: qsTr( "Node 2")
                }
                TabButton {
                    text: qsTr( "Node 3")
                }
                TabButton {
                    text: qsTr( "Node 4")
                }
            }
        }
        /*------*/
        NumberAnimation on y
        {
            id: ani_page_sensor_true
            running: false
            to: 0
            duration: 100
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: ani_page_sensor_false
            running: false
            to: -530
            duration: 500
            //loops: Animation.Infinite
        }
        AnimatedImage {
            id: animatedImage4
            x: 744
            y: 382
            width: 56
            height: 58
            source: "../Image/back.png"
            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    console.info("back")
                    ani_page_sensor_false.start()
                    reg_sensor_true.start()
                    reg_time_true.start()
                    reg_chart_true.start()
                    reg_setting_true.start()
                    reg_line_true.start()
                    txt_lora_true.start()
                    image_signal_true.start()
                }
            }
        }
    }
    /*--- page setting ---*/
    Rectangle {
        id: rectangle9
        x: 0
        y: 667
        width: 800
        height: 480
        color: "#242424"
        border.color: "#242424"
        Rectangle {
            id: rectangle10
            x: -164
            y: -48
            width: 365
            height: 133
            color: "#777777"
            rotation: 135
        }
        NumberAnimation on y
        {
            id: ani_page_setting_true
            running: false
            to: 0
            duration: 100
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: ani_page_setting_false
            running: false
            to: 667
            duration: 500
            //loops: Animation.Infinite
        }
        Text {
            id: txt2
            x: 9
            y: 21
            width: 102
            height: 53
            color: "#ffffff"
            text: qsTr("SETIING")
            font.pixelSize: 20
            rotation: -45
            font.bold: true
        }

        AnimatedImage {
            id: animatedImage6
            x: 736
            y: 8
            width: 56
            height: 58
            source: "../Image/back.png"
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    console.info("back")
                    ani_page_setting_false.start()
                    reg_sensor_true.start()
                    reg_time_true.start()
                    reg_chart_true.start()
                    reg_setting_true.start()
                    reg_line_true.start()
                    txt_lora_true.start()
                    image_signal_true.start()
                }
            }
        }
    }
    /*--- page chart ---*/
    Rectangle {
        id: rectangle11
        x: 0
        y: -1248
        width: 800
        height: 480
        color: "#242424"
        border.color: "#242424"
        NumberAnimation on y
        {
            id: ani_page_chart_true
            running: false
            to: 0
            duration: 100
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: ani_page_chart_false
            running: false
            to: -1248
            duration: 500
            //loops: Animation.Infinite
        }
        Page
        {
            anchors.fill: parent
            SwipeView {
                id: swipeView1
                anchors.fill: parent
                currentIndex: tabBar.currentIndex

                Node_1 {

                }

                Node_2 {
                    Label {
                        text: qsTr("Second page")
                        anchors.centerIn: parent
                    }
                }
            }

            footer: TabBar {
                id: tabBar1
                currentIndex: swipeView.currentIndex
                TabButton {
                    text: qsTr("First")
                }
                TabButton {
                    text: qsTr( "Second")
                }
            }
        }
        /*
        Rectangle {
            id: rectangle12
            x: -164
            y: -48
            width: 365
            height: 133
            color: "#777777"
            rotation: 135
        }
        Text {
            id: txt3
            x: 9
            y: 21
            width: 102
            height: 53
            color: "#ffffff"
            text: qsTr("CHART")
            font.pixelSize: 20
            rotation: -45
            font.bold: true
        }
        */
        AnimatedImage {
            id: animatedImage7
            x: 736
            y: 8
            width: 56
            height: 58
            source: "../Image/back.png"
            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    console.info("back")
                    ani_page_chart_false.start()
                    reg_sensor_true.start()
                    reg_time_true.start()
                    reg_chart_true.start()
                    reg_setting_true.start()
                    reg_line_true.start()
                    txt_lora_true.start()
                    image_signal_true.start()
                }
            }
        }
    }
    /*--- signal ---*/
    AnimatedImage {
        id: animatedImage5
        x: 746
        y: -40
        width: 42
        height: 40
        source: "../Image/wifi_off.png"
        NumberAnimation on y
        {
            id: image_signal_true
            running: false
            to: 5
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: image_signal_false
            running: false
            to: -40
            duration: 100
            //loops: Animation.Infinite
        }
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                setting_lora_true.start()
                setting_lora = true
            }
        }
    }
    /*--- setting connect lora ---*/
    Rectangle {
        id: rectangle7
        x: 169
        y: 1226
        width: 462
        height: 289
        color: "#385592"
        NumberAnimation on y
        {
            id: setting_lora_true
            running: false
            to: 96
            duration: 500
            //loops: Animation.Infinite
        }
        NumberAnimation on y
        {
            id: setting_lora_false
            running: false
            to: 1226
            duration: 100
            //loops: Animation.Infinite
        }
        AnimatedImage {
            id: close
            x: 398
            y: 8
            width: 56
            height: 58
            source: "../Image/close.png"
            MouseArea {
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
                anchors.topMargin: 0
                anchors.fill: parent
                onClicked:
                {
                    setting_lora_false.start()
                    setting_lora = false
                }
            }
        }

    }
}



/*##^##
Designer {
    D{i:0;formeditorZoom:0.8999999761581421}D{i:41;customId:""}D{i:48;customId:""}D{i:49;customId:""}
D{i:50;customId:""}D{i:45;customId:""}D{i:47;customId:""}D{i:43;customId:""}D{i:52;customId:""}
D{i:51;customId:""}D{i:53;customId:""}
}
##^##*/
