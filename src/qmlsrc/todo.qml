import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1

Rectangle {
    id: root

    width: 400
    height: 500

    color: isDark ? "#19232D" : "white"

    property int iconW: 12
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

    function getColor() {
        var strColor

        if (isDark)
            strColor = "#455364"
        else
            strColor = "#ffffff"

        return strColor
    }

    function getText1FontColor() {

        if (isDark)
            return "#bbbbbb"
        else
            return "#777777"
    }

    function getFontColor() {

        if (isDark)
            return "white"
        else
            return "black"
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
            property int myw: m_width - 0
            width: myw
            height: rectan.getItemHeight()
            contentWidth: myw + donebtn.width + flagColor.width
            contentHeight: rectan.getItemHeight()
            boundsBehavior: Flickable.StopAtBounds //该属性设置过后，边界不会被拉出

            Rectangle {
                id: rectan

                anchors.fill: parent
                width: parent.width
                height: getItemHeight()
                border.width: isDark ? 0 : 1
                border.color: "lightgray"
                radius: 0
                //选中颜色设置
                color: view.currentIndex === index ? "lightblue" : getColor()

                //color: view.currentIndex === index ? "#DCDCDC" : "#ffffff"
                function getItemHeight() {
                    var item0H
                    var item1H
                    var item2H
                    var item3H

                    if (text1.visible == false)
                        item0H = 0
                    else
                        item0H = text1.contentHeight

                    if (text2.visible == false)
                        item1H = 0
                    else
                        item1H = text2.contentHeight

                    if (text3.visible == false)
                        item2H = 0
                    else
                        item2H = text3.contentHeight

                    if (text4.visible == false)
                        item3H = 0
                    else
                        item3H = text4.contentHeight

                    return item0H + item1H + item2H + item3H + text2.height + 5
                }

                Rectangle {
                    id: flagColor
                    height: rectan.getItemHeight() - 0
                    width: 6
                    radius: 2
                    anchors.leftMargin: 1
                    color: getPriorityColor(type)
                    Text {
                        anchors.centerIn: parent
                    }
                    visible: true
                }

                RowLayout {
                    id: idlistElemnet
                    height: parent.height
                    width: parent.width
                    spacing: 2
                    Layout.fillWidth: true

                    ColumnLayout {
                        height: parent.height
                        width: parent.width - flagColor.width - donebtn.width - 4
                        spacing: 2
                        Layout.fillWidth: false
                        anchors.leftMargin: 0
                        anchors.rightMargin: 0

                        RowLayout {

                            id: row1

                            function showImg() {

                                var str1 = text1.text.substring(0, 5)
                                var str2 = text1.text.substring(0, 4)
                                if (str1 === "Alarm" || str2 === "定时提醒")
                                    return true
                                else
                                    return false
                            }

                            Rectangle {
                                height: parent.height
                                width: 5
                                visible: text1Img.visible
                            }

                            Image {
                                id: text1Img

                                width: itemheight - 2
                                height: text1.contentHeight
                                fillMode: Image.NoOption
                                horizontalAlignment: Image.AlignHCenter
                                verticalAlignment: Image.AlignVCenter

                                smooth: true
                                sourceSize.height: itemheight - 2
                                sourceSize.width: itemheight - 2
                                source: "/res/time.svg"

                                visible: row1.showImg()
                            }
                            TextArea {
                                id: text1
                                width: parent.width
                                color: view.currentIndex === index ? "black" : getText1FontColor()
                                font.pointSize: FontSize - 2
                                                > maxFontSize ? maxFontSize : FontSize - 2
                                font.bold: true
                                wrapMode: Text.Wrap
                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                                text: time
                                visible: true
                            }
                        }

                        TextArea {
                            id: text2
                            width: parent.width
                            wrapMode: Text.Wrap
                            text: type

                            visible: false
                        }

                        TextArea {
                            id: text3
                            width: parent.width
                            font.pointSize: FontSize
                            wrapMode: Text.Wrap
                            color: view.currentIndex === index ? "black" : getFontColor()
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            text: dototext

                            visible: true
                        }

                        TextArea {
                            id: text4
                            width: parent.width
                            wrapMode: Text.Wrap
                            text: itemheight

                            visible: false
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
                        m_Todo.reeditText()
                        var data = view.model.get(view.currentIndex)
                        console.log(data.time + "," + data.dototext + ", count=" + view.count)
                    }

                    onPressed: {
                        donebtn.visible = true
                        //console.log("mouse pressed")
                    }
                }

                Rectangle {
                    id: donebtn
                    height: parent.height - 0
                    width: 45
                    color: "red"
                    anchors.right: parent.right
                    visible: isBtnVisible

                    Image {
                        id: doneImg

                        width: donebtn.width
                        height: donebtn.width
                        x: -2
                        y: (donebtn.height - donebtn.width) / 2
                        fillMode: Image.NoOption
                        horizontalAlignment: Image.AlignHCenter
                        verticalAlignment: Image.AlignVCenter

                        smooth: true
                        sourceSize.height: donebtn.width
                        sourceSize.width: donebtn.width
                        source: "/res/todo_done.png"

                        visible: true
                    }

                    Text {
                        anchors.centerIn: parent
                        //text: qsTr("Done")
                        color: "#ffffff"
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            view.currentIndex = index

                            m_Todo.addToRecycle()
                            view.model.remove(index)
                            m_Todo.refreshTableLists()
                            m_Todo.refreshAlarm()
                            m_Todo.saveTodo()
                            console.log("Done isclick")
                        }
                    }
                }
            }
        }
    }

    function getPriorityColor(ntype) {
        switch (ntype) {
        case 0:
            return "gray"
        case 1:
            return "red"
        case 2:
            return "orange"
        case 3:
            return "blue"
        default:
            return "black"
        }
    }
}
