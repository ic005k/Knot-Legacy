import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1

Rectangle {
    id: root

    width: 400
    height: 500

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

    function insertItem(strTime, type, strText, height, curIndex) {
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

    ListView {
        id: view
        width: parent.width
        height: parent.height

        anchors {
            fill: parent
            margins: 0
        }
        spacing: 4
        cacheBuffer: 50
        model: TodoModel {}

        ScrollBar.vertical: ScrollBar {
            width: 8
            policy: ScrollBar.AsNeeded
        }

        delegate: Flickable {
            id: flack
            property int myw: m_width - 8
            width: myw
            height: itemheight
            contentWidth: myw + donebtn.width + 5
            contentHeight: itemheight
            boundsBehavior: Flickable.StopAtBounds //该属性设置过后，边界不会被拉出

            Rectangle {
                id: rectan
                anchors.fill: parent
                width: parent.width
                height: itemheight
                border.width: 1
                border.color: "lightsteelblue"
                radius: 0
                //color: ListView.isCurrentItem ? "#DCDCDC" : "#ffffff" //选中颜色设置
                color: view.currentIndex === index ? "#DCDCDC" : "#ffffff"

                RowLayout {
                    id: idlistElemnet
                    height: parent.height
                    width: parent.width
                    spacing: 2
                    Layout.fillWidth: true

                    Rectangle {
                        id: flagColor
                        height: itemheight - 6
                        width: 6
                        radius: 2
                        anchors.leftMargin: 1
                        color: getListEleHeadColor(type)
                        Text {
                            anchors.centerIn: parent
                        }
                    }

                    ColumnLayout {
                        height: parent.height
                        width: parent.width - flagColor.width - donebtn.width - 4
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
                    anchors.fill: parent
                    onClicked: {
                        view.currentIndex = index //实现item切换
                        //console.log("index=" + index + "  c_index=" + ListView.isCurrentItem)
                    }

                    onDoubleClicked: {
                        mydlgTodo.reeditText()
                        var data = view.model.get(view.currentIndex)
                        console.log(data.time + "," + data.dototext + ", count=" + view.count)
                    }
                }

                Rectangle {
                    id: donebtn
                    height: parent.height - 2
                    width: 50
                    color: "red"
                    anchors.right: parent.right

                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Done")
                        color: "#ffffff"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            view.currentIndex = index

                            mydlgTodo.addToRecycle()
                            view.model.remove(index)
                            mydlgTodo.refreshAlarm()
                            console.log("mouse isclick")
                        }
                    }
                }
            }
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
