import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1

Rectangle {
    id: root

    width: 500
    height: 400

    property int itemCount: 0
    property bool isHighPriority: false

    function isAlarm(index) {

        return isHighPriority
    }

    function setHighPriority(isFalse) {
        isHighPriority = isFalse
    }

    function setCurrentItem(currentIndex) {
        view.currentIndex = currentIndex
    }

    function getCurrentIndex() {
        return view.currentIndex
    }

    function getItemCount() {
        itemCount = view.count
        console.log("count=" + itemCount)
        return itemCount
    }

    function getItemText(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.time + "|=|" + data.dototext
    }

    function getTime(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.time
    }

    function getTodoText(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.dototext
    }

    function getType(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.type
    }

    function addItem(strTime, type, strText, height) {
        view.model.append({
                              "time": strTime,
                              "type": type,
                              "dototext": strText,
                              "itemheight": height
                          })
    }

    function insertRecycle(strTime, type, strText, height, curIndex) {
        view.model.insert(curIndex, {
                              "time": strTime,
                              "type": type,
                              "dototext": strText,
                              "itemheight": height
                          })
    }

    function delItem(currentIndex) {
        view.model.remove(currentIndex)
    }

    function modifyItem(currentIndex, strTime, strText) {

        view.model.setProperty(currentIndex, "time", strTime)
        view.model.setProperty(currentIndex, "dototext", strText)
    }

    function modifyItemTime(currentIndex, strTime) {

        view.model.setProperty(currentIndex, "time", strTime)
    }

    function modifyItemType(currentIndex, type) {

        view.model.setProperty(currentIndex, "type", type)
    }

    function modifyItemText(currentIndex, strText) {
        view.model.setProperty(currentIndex, "dototext", strText)
    }

    Component {
        id: dragDelegate

        Rectangle {
            id: listItem
            width: ListView.view.width
            height: itemheight
            color: ListView.isCurrentItem ? "#DCDCDC" : "#ffffff" //选中颜色设置

            border.width: 1
            border.color: "lightsteelblue"

            radius: 0

            RowLayout {

                id: idlistElemnet
                height: parent.height
                width: parent.width
                spacing: 2
                Layout.fillWidth: true

                Rectangle {
                    height: itemheight - 6
                    width: 6
                    radius: 2
                    anchors.leftMargin: 1
                    color: getListEleHeadColor(type)
                    //visible: isAlarm(view.currentIndex)
                    Text {
                        anchors.centerIn: parent
                    }
                }

                ColumnLayout {
                    height: parent.height
                    width: parent.width - 6
                    spacing: 2
                    Layout.fillWidth: true

                    anchors.leftMargin: 0
                    anchors.rightMargin: 0

                    TextArea {
                        id: text1
                        color: "gray"
                        font.pixelSize: FontSize - 2
                        font.bold: true
                        readOnly: true
                        width: parent.width
                        wrapMode: TextArea.Wrap
                        text: time
                    }
                    TextArea {
                        id: text2
                        readOnly: true
                        visible: false
                        width: parent.width
                        wrapMode: TextArea.Wrap
                        text: type
                    }
                    TextArea {
                        id: text3
                        font.pixelSize: FontSize
                        readOnly: true
                        width: parent.width
                        wrapMode: TextArea.Wrap
                        color: isHighPriority ? "#EF5B98" : "#000000"
                        text: dototext
                    }
                    TextArea {
                        id: text4
                        readOnly: true
                        visible: false
                        width: parent.width
                        wrapMode: TextArea.Wrap
                        text: itemheight
                    }
                }
            }

            MouseArea {

                property point clickPos: "0,0"

                anchors.fill: parent
                onPressed: {
                    clickPos = Qt.point(mouse.x, mouse.y)
                }
                onReleased: {
                    var delta = Qt.point(mouse.x - clickPos.x,
                                         mouse.y - clickPos.y)
                    console.debug("delta.x: " + delta.x)
                    if ((delta.x < 0) && (aBtnShow.running === false)
                            && (delBtn.width == 0)) {
                        aBtnShow.start()
                    } else if (aBtnHide.running === false
                               && (delBtn.width > 0)) {
                        aBtnHide.start()
                    }
                }

                onClicked: {

                    view.currentIndex = index //实现item切换
                    console.log("ItemHeight=" + text1.contentHeight + text3.contentHeight)
                }

                onDoubleClicked: {
                    mydlgTodo.reeditText()
                    var data = view.model.get(view.currentIndex)
                    console.log(data.time + "," + data.dototext + ", count=" + view.count)
                }
            }

            Rectangle {
                color: "#AAAAAA"
                height: 0
                width: parent.width
                anchors.bottom: parent.bottom
            }

            Rectangle {
                id: delBtn
                visible: true
                height: parent.height
                width: 0
                color: "#FF0000"

                anchors.right: parent.right
                anchors.rightMargin: -30
                radius: 0

                Text {
                    width: 56
                    anchors.centerIn: parent

                    text: qsTr("Done")
                    color: "#ffffff"
                }


                /*Image {
                    id: delBtn00
                    anchors.centerIn: parent

                    width: 0
                    height: 40
                    fillMode: Image.scale
                    horizontalAlignment: Image.AlignLeft
                    verticalAlignment: Image.AlignTop

                    smooth: true
                    source: "/tab.png"
                }*/
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        mydlgTodo.addToRecycle()
                        view.model.remove(index)
                    }
                }
            }

            PropertyAnimation {
                id: aBtnShow
                target: delBtn
                property: "width"
                duration: 100
                from: 0
                to: 80
            }
            PropertyAnimation {
                id: aBtnHide
                target: delBtn
                property: "width"
                duration: 100
                from: 80
                to: 0
            }
        }
    }

    ListView {
        id: view

        anchors {
            fill: parent
            margins: 4
        }

        model: TodoModel {}
        delegate: dragDelegate

        spacing: 4
        cacheBuffer: 50

        ScrollBar.vertical: ScrollBar {
            width: 8
            policy: ScrollBar.AsNeeded
        }
    }

    function getListEleHeadColor(ntype) {
        switch (ntype) {
        case 0:
            return "lightgray"
        case 1:
            return "red"
        case 2:
            return "orange"
        case 3:
            return "lightblue"
        default:
            return "black"
        }
    }
}
