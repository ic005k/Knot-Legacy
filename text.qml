import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12
import MyModel 1.0

Item {
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
            //width: 6
            //position: textPos
            id: control
            size: 0.3
            position: 0.2
            active: true
            orientation: Qt.Vertical
            anchors.right: parent.right
            policy: ScrollBar.AsNeeded

            contentItem: Rectangle {
                implicitWidth: 4
                implicitHeight: 76
                radius: width / 2
                color: control.pressed ? "#81e889" : "#c2f4c6"
            }

            Component.onCompleted: {
                console.log(textArea.lineCount)
                console.log(textArea.height)
                console.log(control.position)
                console.log(textArea.y)
            }
        }

        property int rowHeight: textArea.font.pointSize + 20
        property int marginsTop: 10
        property int marginsLeft: 4
        property int lineCountWidth: 40
        property real textHeight: textArea.contentHeight
    }
}
