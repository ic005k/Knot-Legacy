import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.0
import QtQuick.Layouts 1.3
//import QtWebView 1.0
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

    function loadHtmlBuffer(strhtml) {

        document.loadBuffer(strhtml)
    }

    function setVPos(vpos) {
        flickable.contentY = vpos
        console.log(vpos)
    }

    function getVPos() {
        file.textPos = contentY
        console.log(file.textPos)
    }

    function move(x0) {
        x = x + x0
        file.curX = x
        console.log(file.curX)
    }

    function setX(x0) {
        x = 0

    }

    function getText()
    {
        file.source = textArea.text
    }

    DocumentHandler {
        id: document
        objectName: "dochandler"
        document: textArea.textDocument
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd
        //textColor: "#FF0000"

        //Component.onCompleted: document.load("qml:/texteditor.html")
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
        height: myH
        //fillMode: Image.TileHorizontally
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop

        smooth: true
        source: "/1res/b.png"
    }

    Flickable {
        id: flickable
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent

        states: State {
            name: "autoscroll"
            PropertyChanges {
                target: flickable
            }
        }

        onMovementEnded: {
            state = "autoscroll"
            file.textPos = contentY
            console.log(file.textPos)
        }

        TextArea.flickable: TextArea {
            id: textArea

            font.pixelSize: FontSize
            font.family: FontName
            font.letterSpacing: 2
            renderType: Text.NativeRendering
            font.hintingPreference: Font.PreferVerticalHinting
            textFormat: Qt.AutoText

            //onTextChanged: file.text = text
            //Component.onCompleted: text = file.text
            wrapMode: TextArea.Wrap
            readOnly: isReadOnly
            focus: true
            persistentSelection:false
            selectByMouse: isBySelect
            smooth: true

            color: "#664E30"

            text: strText
            visible: true


            MouseArea {
                acceptedButtons: Qt.RightButton
                anchors.fill: parent
                onClicked: contextMenu.open()
            }

            onLinkActivated: {
                //Qt.openUrlExternally(link)
                document.setBackDir(link)
                //document.load("file://" + htmlPath + link)
                document.setReadPosition(link)
                console.log(htmlPath + link)
                console.log(htmlPath)
                console.log(link)
            }

            PropertyAnimation on x {
                easing.type: Easing.Linear
                running: false
                from: 300
                to: 0
                duration: 200
                loops: 1 //Animation.Infinite
            }


            SequentialAnimation on opacity {
                //应用于透明度上的序列动画
                running: true
                loops: 1 //Animation.Infinite //无限循环
                NumberAnimation {
                    from: 0
                    to: 1
                    duration: 500
                } //淡出效果
                PauseAnimation {
                    duration: 0
                }
            }
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
                implicitWidth: 15
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

    Menu {
        id: contextMenu

        MenuItem {
            text: qsTr("Copy")
            enabled: textArea.selectedText
            onTriggered: textArea.copy()
        }


        MenuItem {
            text: qsTr("Cut")
            enabled: textArea.selectedText
            onTriggered: textArea.cut()
        }

        MenuItem {
            text: qsTr("Paste")
            enabled: textArea.canPaste
            onTriggered: textArea.paste()
        }
    }
}
