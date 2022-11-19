import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    width: 500
    height: 400

    property int itemH: 32
    property int itemCount: 0
    property bool isHighPriority: false

    function gotoEnd() {
        view.positionViewAtEnd()
    }

    function gotoBeginning() {
        view.positionViewAtBeginning()
    }

    function gotoIndex(index) {
        view.positionViewAtIndex(index, Tumbler.Center)
    }

    function isAlarm(index) {
        //mydlgTodo.isAlarm(index)
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

    function getText0(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.text0
    }

    function getTop(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.text_top
    }

    function getType(itemIndex) {
        var data = view.model.get(itemIndex)
        return data.type
    }

    function addItem(text0, text1, text2, type, text_top) {
        view.model.append({
                              "text0": text0,
                              "text1": text1,
                              "text2": text2,
                              "type": type,
                              "text_top": text_top
                          })
    }

    function insertItem(strTime, type, strText, curIndex) {
        view.model.insert(curIndex, {
                              "time": strTime,
                              "type": type,
                              "dototext": strText
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
            height: itemH
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
                    height: itemH - 2
                    width: 6
                    radius: 2
                    anchors.leftMargin: 1
                    color: getListEleHeadColor(type)

                    Text {
                        anchors.centerIn: parent
                    }
                }

                Text {
                    id: item0
                    width: parent.width
                    wrapMode: TextArea.NoWrap
                    font.bold: type
                    text: text0
                }

                Text {
                    id: item1
                    Layout.preferredWidth: parent.width / 4
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    width: parent.width
                    wrapMode: TextArea.NoWrap
                    color: isHighPriority ? "#EF5B98" : "#000000"
                    font.bold: type
                    text: text1
                }

                Text {
                    id: item2
                    Layout.preferredWidth: parent.width / 3
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignRight
                    elide: Text.ElideRight
                    width: parent.width
                    wrapMode: TextArea.NoWrap
                    font.bold: type
                    text: text2
                    anchors.rightMargin: 10
                }

                TextArea {
                    id: item_type
                    visible: false
                    width: parent.width
                    wrapMode: TextArea.NoWrap
                    text: type
                }

                Text {
                    id: item_top
                    visible: false
                    width: parent.width
                    wrapMode: TextArea.NoWrap
                    text: text_top
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
                    mw_one.clickData()
                }

                onDoubleClicked: {
                    mw_one.reeditData()
                    var data = view.model.get(view.currentIndex)
                    console.log(data.text0 + "," + data.type + ", count=" + view.count)
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
                visible: false
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

        model: MainModel {}
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
            return "yellow"
        case 3:
            return "lightblue"
        default:
            return "black"
        }
    }
}
