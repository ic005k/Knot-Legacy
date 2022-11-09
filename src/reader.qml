import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import MyModel1 1.0
import MyModel2 1.0

Rectangle {
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

    function getVHeight() {
        file.textHeight = textArea.contentHeight
        console.log(file.textHeight)
    }

    function getSelectedText() {
        console.log("selectedText=" + textArea.selectedText)
        return textArea.selectedText
    }

    function move(x0) {
        x = x + x0
        file.curX = x
        console.log(file.curX)
    }

    function setX(x0) {
        x = 0
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
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop

        smooth: true
        source: backImgFile
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
            file.textHeight = contentHeight
            console.log(file.textPos)
            console.log(file.textHeight)
        }

        TextArea.flickable: TextArea {
            id: textArea
            visible: !isWebViewShow
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
            focus: true
            persistentSelection: isSelText
            selectByMouse: isSelText
            smooth: true

            color: myTextColor

            text: strText

            onLinkActivated: {
                //Qt.openUrlExternally(link)
                document.setBackDir(link)
                //document.load("file://" + htmlPath + link)
                document.setReadPosition(link)
                console.log(htmlPath + link)
                console.log(htmlPath)
                console.log("isLink=" + link)
            }
            onReleased: {
                mw_one.on_SetReaderFunVisible()
            }

            MouseArea {
                id: mouse_area
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.RightButton
                onClicked: {

                    //contextMenu.open()
                }
                onPressAndHold: {

                    //mw_one.on_btnSelText_clicked()
                }
            }

            onActiveFocusOnPressChanged: {
                Qt.inputMethod.hide()
            }

            PropertyAnimation on x {
                easing.type: Easing.Linear
                running: isAni
                from: aniW
                to: 0
                duration: 200
                loops: 1 //Animation.Infinite
            }


            /*SequentialAnimation on opacity {
                //应用于透明度上的序列动画
                running: isAni
                loops: 1 //Animation.Infinite //无限循环
                NumberAnimation {
                    from: 0
                    to: 1
                    duration: 1000
                } //淡出效果
                PauseAnimation {
                    duration: 0
                } //暂停400ms
            }*/
        }

        ScrollBar.vertical: ScrollBar {
            id: vbar
            position: 0.2
            policy: ScrollBar.AsNeeded
            width: 7
            hoverEnabled: true
            active: hovered || pressed
            orientation: Qt.Vertical
            size: frame.height / content.height
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom


            /*contentItem: Rectangle {
                id: slider
                implicitWidth: 4
                implicitHeight: 76
                radius: width / 2
                color: vbar.pressed ? "#e7e7e7" : "#1296db"
                opacity: (vbar.policy === ScrollBar.AlwaysOn
                          || vbar.size < 1.0) ? 1.0 : 0.0
            }*/
        }

        Component.onCompleted: {
            console.log("lineCount=" + textArea.lineCount)
            console.log("textHeight=" + textArea.height)
            console.log("textPosition=" + vbar.position)
        }
    }


    /*Menu {
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
    }*/
}
