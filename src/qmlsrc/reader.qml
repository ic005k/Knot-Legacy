import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import MyModel2 1.0

Rectangle {
    id: textitem
    visible: true
    width: myW
    height: myH

    property string strUrl: ""
    property string strText: ""
    property string pdfFile: ""
    property bool isPDF: false
    property bool isEPUBText: false

    function loadText(str) {
        strText = str
        isPDF = false
        isEPUBText = true
    }

    function loadHtml(htmlFile, skipID) {

        document.load("file://" + htmlFile)
        isPDF = false
        isEPUBText = true
        if (skipID !== "") {

        }
    }

    function loadHtmlStr(str) {

        strText = str
        isPDF = false
        isEPUBText = true
    }

    function loadPDF(pdffile) {
        pdfFile = "file:///" + pdffile
        isPDF = true
        isEPUBText = false
        console.debug(pdfFile)
    }

    function loadHtmlBuffer(strhtml) {

        document.loadBuffer(strhtml)
        isEPUBText = true
    }

    function setVPos(vpos) {
        flickable.contentY = vpos
        console.log(vpos)
    }

    function getVPos() {

        return flickable.contentY
    }

    function getVHeight() {

        return textArea.contentHeight
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

    function getBookmarkText() {
        var x = 0
        var y = flickable.contentY

        for (var i = 1; i < 20; i++) {
            var start = textArea.positionAt(x, y + FontSize * i)
            var end = textArea.positionAt(x + textArea.width,
                                          y + FontSize * 4 * i)
            var txt = textArea.getText(start, end)
            txt = txt.trim()
            if (txt.length > 0) {
                break
            }
        }
        if (txt === "") {
            txt = "Bookmark"
        }

        console.log("bookmark txt=" + txt + "  x=" + x + "  start=" + start + "  end=" + end)

        return txt
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
        }
        onError: {
            errorDialog.text = message
            errorDialog.visible = true
        }
    }

    Image {
        id: m_Image
        width: myW
        height: myH
        fillMode: Image.Tile
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop

        smooth: true
        source: backImgFile
        visible: backImgFile === "" ? false : true

        MouseArea {
            id: mouse_area
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton
            propagateComposedEvents: false
            onClicked: {

                var my = mouseY - flickable.contentY
                console.log("clicked..." + mouseY + " " + my + myH)

                if (my < myH / 3) {
                    m_Reader.setPageScroll0()
                }

                if (my > myH / 3 && my < (myH * 2) / 3) {
                    m_Reader.setPanelVisible()
                }

                if (my > (myH * 2) / 3) {
                    m_Reader.setPageScroll1()
                }
            }
            onDoubleClicked: {
                console.log("double...")
                m_Reader.selectText()
            }
            onPressAndHold: {
                console.log("press and hold...")
            }
        }
    }

    Rectangle {
        id: m_Rect
        width: myW
        height: myH
        color: myBackgroundColor
        visible: backImgFile === "" ? true : false
    }

    Flickable {
        id: flickable
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds

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
            visible: isEPUBText
            font.pixelSize: FontSize
            font.family: FontName
            font.letterSpacing: 2
            renderType: Text.NativeRendering
            font.hintingPreference: Font.PreferVerticalHinting
            textFormat: Qt.AutoText
            cursorPosition: nCursorPos

            wrapMode: TextArea.Wrap
            readOnly: true
            focus: true
            persistentSelection: isSelText
            selectByMouse: isSelText
            smooth: true
            color: myTextColor
            text: strText

            onLinkActivated: {
                console.log("reader htmlPath=" + htmlPath)
                console.log("reader Link=" + link)
                document.setBackDir(link)
                document.parsingLink(link)
            }

            onReleased: {

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
                loops: 1
            }

            SequentialAnimation on opacity {
                //应用于透明度上的序列动画
                running: false
                loops: 1 //Animation.Infinite //无限循环
                NumberAnimation {
                    from: 0
                    to: 1
                    duration: 1000
                } //淡出效果
                PauseAnimation {
                    duration: 0
                } //暂停400ms
            }
        }

        ScrollBar.vertical: ScrollBar {
            id: vbar
            position: 0.2
            policy: ScrollBar.AsNeeded
            width: 8
            hoverEnabled: true
            active: hovered || pressed
            orientation: Qt.Vertical
            size: 0.3
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            //contentItem: Rectangle {
            //    color: "#1E90FF"
            //}
        }

        Component.onCompleted: {

        }
    }
}
