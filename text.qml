import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12
import QtWebView 1.0
import MyModel1 1.0
import MyModel2 1.0

Item {
    id: textitem
    visible: true
    width: myW
    height: myH

    File {
        id: file
        source: "" //"/Users/hz/Documents/1.txt"
    }

    function loadHtml(msg) {

        document.load("file://" + msg)
    }

    function setVPos(vpos) {
        flickable.contentY = vpos
        console.log(vpos)
    }

    DocumentHandler {
        id: document
        objectName: "dochandler"
        document: textArea.textDocument
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd

        // textColor: TODO
        //Component.onCompleted: document.load("qrc:/texteditor.html")
        //Component.onCompleted: document.load("file://" + htmlFile)
        onLoaded: {
            textArea.text = text
        }
        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }

    Image {
        id: rocket
        width: myW
        height: 2000
        //fillMode: Image.TileHorizontally
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop

        smooth: true
        source: "/src/b.png"
    }

    Flickable {
        id: flickable
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent

        WebView {
            id: mywebview
            anchors.fill: parent
            url: "file://" + baseUrl
            visible: false
        }

        states: State {
            name: "autoscroll"
            PropertyChanges {
                target: flickable
                //contentY: textPos
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
            font.family: FontName
            font.letterSpacing: 2
            renderType: Text.NativeRendering
            font.hintingPreference: Font.PreferVerticalHinting
            textFormat: Qt.AutoText

            //onTextChanged: file.text = text
            //Component.onCompleted: text = file.text
            wrapMode: TextArea.Wrap
            readOnly: true
            persistentSelection: true
            //selectByMouse: true
            smooth: true

            color: "#000000"

            text: strText
            visible: true

            //leftPadding: 2
            //rightPadding: 2
            topPadding: 0
            bottomPadding: 0

            onLinkActivated: Qt.openUrlExternally(link)
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
                color: control.pressed ? "#e7e7e7" : "#1296db"
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
