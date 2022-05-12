import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12
import MyModel 1.0

Item {
    id: textitem
    visible: true
    width: 400
    height: 800

    File {
        id: file
        source: "" //"/Users/hz/Documents/1.txt"
    }

    Image {
        id: rocket
        fillMode: Image.TileHorizontally
        smooth: true
        source: "/src/b.png"
    }

    Flickable {
        id: flickable
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent


        states: State {
            name: "autoscroll"
            PropertyChanges {
                target: flickable
                contentY: textPos
            }
        }

        onMovementEnded: {
            state = "autoscroll"
            file.textPos = contentY
            console.log(file.textPos)
        }

        TextArea.flickable: TextArea {
            id: textArea
            //anchors.fill: parent //QML TextArea: Possible anchor loop detected on fill.
            font.pixelSize: FontSize
            font.letterSpacing: 2
            renderType: Text.NativeRendering
            font.hintingPreference: Font.PreferVerticalHinting
            textFormat: Qt.AutoText

            //onTextChanged: file.text = text
            //Component.onCompleted: text = file.text
            wrapMode: TextArea.Wrap
            readOnly: true
            color: "#000000"

            text: strText
        }

        ScrollBar.vertical: ScrollBar {
            id: control
            size: 0.3
            position: 0.2
            active: true
            orientation: Qt.Vertical
            anchors.right: parent.right
            policy: ScrollBar.AsNeeded

            contentItem: Rectangle {
                id: slider
                implicitWidth: 2
                implicitHeight: 76
                radius: width / 2
                color: control.pressed ? "#e7e7e7" : "#858585"
                opacity: (control.policy === ScrollBar.AlwaysOn
                          || control.size < 1.0) ? 1.0 : 0.0
            }
        }

        Component.onCompleted: {
            console.log(textArea.lineCount)
            console.log(textArea.height)
            console.log(control.position)
        }
    }
}
