import QtQuick 2.15
import QtQuick.Controls 2.15

import QtWebEngine 1.10

import MyModel2 1.0

Item {
    id: textitem
    visible: true

    function loadHtml(htmlfile) {
        document.load("file://" + htmlfile)
    }

    function loadHtmlBuffer(strhtml) {

        document.loadBuffer(strhtml)
    }

    function setVPos(vpos) {
        if (vpos < 0)
            vpos = 0
        flickable.contentY = vpos
        console.debug("set vpos=" + vpos)
    }

    function getVPos() {
        console.debug("get vpos=" + flickable.contentY)
        return flickable.contentY
    }

    function getVHeight() {

        return flickable.contentHeight
    }

    function move(x0) {
        x = x + x0
        file.curX = x
        console.log(file.curX)
    }

    function setX(x0) {
        x = 0
    }

    function getText() {
        file.source = textArea.text
    }

    function setWebViewFile(htmlfile) {
        webView.url =  Qt.resolvedUrl("file:///" + htmlfile)
    }

    DocumentHandler {
        id: document
        objectName: "dochandler"
        document: textArea.textDocument
        cursorPosition: textArea.cursorPosition
        selectionStart: textArea.selectionStart
        selectionEnd: textArea.selectionEnd

        onLoaded: {
            textArea.text = text
            console.debug("Load Notes Done...")
            m_Notes.setVPos()
        }
        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }

    WebEngineView {
        id: webView
        anchors.fill: parent
        visible: true
        url: "file:///C:/Users/Administrator/.Knot/memo.html" // 加载本地HTML文件
    }

    Flickable {
        id: flickable
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent
        clip: true
        visible: false

        states: State {
            name: "autoscroll"
            PropertyChanges {
                target: flickable
            }
        }

        onMovementEnded: {
            state = "autoscroll"
        }

        TextArea.flickable: TextArea {
            id: textArea
            visible: true

            renderType: Text.NativeRendering
            font.hintingPreference: Font.PreferVerticalHinting
            textFormat: Qt.AutoText
            wrapMode: TextArea.Wrap
            readOnly: true
            focus: true
            persistentSelection: false
            selectByKeyboard: true
            selectByMouse: false
            smooth: true

            color: isDark ? "#FFFFFF" : "#664E30"

            background: Rectangle {
                color: isDark ? "#19232d" : "#FFFFFF"
                radius: 0
            }

            onLinkActivated: {

                document.parsingLink(link, "note")

                console.debug("qml link=" + link)
            }

            Component.onCompleted: {
                console.debug("Load Notes Component Completed...")
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: control
            size: 0.3
            position: 0.2
            width: 7
            active: true
            orientation: Qt.Vertical
            anchors.right: parent.right
            policy: ScrollBar.AsNeeded
        }
    }
}
