import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import MyModel2 1.0

import EBook.Models 1.0

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

    property var textChunks: [] // 存储分块后的 HTML 片段

    property variant stringList: null

    function updateText(str) {
        textChunks = []
        // 优化分割逻辑，确保 HTML 结构完整
        let rawChunks = str.split(/(?=<div class="page-break")/i)
        // 按分页符分割
        textChunks = rawChunks.filter(chunk => chunk.trim() !== "")
    }

    function getText() {
        return textArea.text
    }

    function setText(str) {
        //textArea.text = str
        updateText(str)
    }

    function loadText(str) {
        //strText = str
        updateText(str)
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

        //strText = str
        updateText(str)
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

        //document.loadBuffer(strhtml)
        //updateText(strhtml)
        textModel.splitContent(strhtml); // 调用C++处理
        isEPUBText = true
    }

    // 修改为基于 ListView 的滚动控制
    function setVPos(vpos) {
        contentListView.contentY = vpos
    }

    function getVPos() {
        return contentListView.contentY
    }

    function getVHeight() {
        return contentListView.contentHeight
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
        // 基于当前可视区域的首个可见项计算
        let firstVisibleIndex = contentListView.indexAt(
                0, contentListView.contentY)
        if (firstVisibleIndex === -1)
            return "Bookmark"

        let delegateItem = contentListView.itemAtIndex(firstVisibleIndex)
        if (!delegateItem)
            return "Bookmark"

        // 获取该段落中坐标对应的字符位置
        let relY = contentListView.contentY - delegateItem.y
        let pos = delegateItem.positionAt(0, relY)
        return delegateItem.text.substring(pos, pos + 100) // 取前100字符
    }

    function setTextAreaCursorPos(nCursorPos) {
        textArea.cursorPosition = nCursorPos
    }

    function handleLinkClicked(link) {
        console.log("Clicked link:", link)
        document.setBackDir(link)
        document.parsingLink(link, "reader")
        // 如果需要保持滚动位置，可在此处记录位置
    }

    DocumentHandler {
        id: document
        objectName: "dochandler"

        //document: textArea.textDocument
        //cursorPosition: textArea.cursorPosition
        //selectionStart: textArea.selectionStart
        //selectionEnd: textArea.selectionEnd
        onLoaded: {
            //textArea.text = text
            updateText(text)
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
    }

    Rectangle {
        id: m_Rect
        width: myW
        height: myH
        color: myBackgroundColor
        visible: backImgFile === "" ? true : false
    }


    /*Flickable {
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
            font.weight: FontWeight
            font.letterSpacing: 2
            renderType: Text.NativeRendering
            font.hintingPreference: Font.PreferVerticalHinting
            textFormat: Qt.AutoText
            cursorPosition: 0

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
                document.parsingLink(link, "reader")
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
            width: 15
            hoverEnabled: true
            active: hovered || pressed
            orientation: Qt.Vertical
            size: 0.3
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: textArea.contentHeight > myH ? true : false

            contentItem: Rectangle {
                color: "#1E90FF"
            }
        }

        Component.onCompleted: {

        }
    }*/
    ListView {
        id: contentListView
        anchors.fill: parent
        //model: textChunks // 数据模型（需新增）
        //model: textChunkModel // 绑定到 C++ 模型
        spacing: 5 // 段落间距
        cacheBuffer: 500 // 预加载区域

        // 声明 C++ 模型
        model: TextChunkModel {
                id: textModel
            }

        clip: true
        delegate: Text {
            width: parent.width - 10 // 留出滚动条空间
            textFormat: Text.RichText // 必须启用富文本
            text: model.text //modelData + (index === textChunks.length - 1 ? "" : "</html>") // 修复分割后的闭合
            wrapMode: Text.Wrap
            font.pixelSize: FontSize
            font.family: FontName
            color: myTextColor

            renderType: Text.NativeRendering // 使用原生渲染引擎

            onLinkActivated: handleLinkClicked(link) // 处理链接点击
        }

        ScrollBar.vertical: ScrollBar {
            width: 10
            policy: contentListView.contentHeight
                    > contentListView.height ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
        }
    }


}
