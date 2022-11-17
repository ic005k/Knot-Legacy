import QtQuick 2.12
import QtQuick.Window 2.12
import QtQml 2.3
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.1

Rectangle {
    id: root

    width: 500
    height: 400

    property int itemH: 120
    property int itemCount: 0

    function clearAllItems()
    {
        itemCount = view.count;
        if(itemCount>0) {
        for(i=0;i<itemCount;i++)
        {
            delItem(0);

        }
        }
    }

    function setCurrentItem(currentIndex)
    {
        view.currentIndex = currentIndex
    }

    function getItemCount(){
        itemCount=view.count
        console.log("count=" + itemCount)
        return itemCount
    }

     function getItemText(itemIndex)
     {
         var data = view.model.get(itemIndex)
        return data.time + "|=|" + data.dototext
     }

     function getTime(itemIndex)
     {
         var data = view.model.get(itemIndex)
        return data.time
     }

     function getTodoText(itemIndex)
     {
         var data = view.model.get(itemIndex)
        return  data.dototext
     }

    function addItem( strTime,strText)
    {

       view.model.append({"time":strTime,"dototext":strText})

    }

    function delItem(currentIndex)
    {
        view.model.remove(currentIndex)
    }

    function modifyItem(currentIndex,strTime,strText)
    {

        view.model.setProperty(currentIndex, "time", strTime)
        view.model.setProperty(currentIndex, "dototext", strText)
    }

    function modifyItemTime(currentIndex,strTime)
    {

        view.model.setProperty(currentIndex, "time", strTime)

    }

    function modifyItemText(currentIndex,strText)
    {
        view.model.setProperty(currentIndex, "dototext", strText)
    }

    Component {
        id: dragDelegate


        /*Rectangle {
            id: content

            anchors {
                left: parent.left
                right: parent.right
            }
            height: column.implicitHeight + 4

            border.width: 1
            border.color: "lightsteelblue"

            radius: 10

            Column {
                id: column
                anchors {
                    fill: parent
                    margins: 2
                }

                Text {
                    text: 'Time: ' + time
                }
                Text {
                    text: 'Text: ' + dototext
                }
            }
        }*/
        Rectangle {
            id: listItem
            width: ListView.view.width
            height: itemH
            color: ListView.isCurrentItem ? "#DCDCDC" : "#ffffff" //选中颜色设置

            /*anchors {
                left: parent.left
                right: parent.right
            }*/

            border.width: 1
            border.color: "lightsteelblue"

            radius: 0

            RowLayout {

                id: idlistElemnet
                height: parent.height
                width: parent.width
                spacing: 5
                Layout.fillWidth: true

                Rectangle {
                    height: itemH - 6
                    width: 10
                    radius: 4
                    color: getListEleHeadColor(1)
                    Text {
                        anchors.centerIn: parent
                    }
                }

                ColumnLayout {
                    height: parent.height
                    width: parent.width
                    spacing: 8
                    Layout.fillWidth: false
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                    TextArea {
                        id: text1
                        width: parent.width
                        wrapMode: TextArea.Wrap
                        text: time
                    }
                    TextArea {
                        id: text2
                        width: parent.width
                        wrapMode: TextArea.Wrap

                        text: dototext
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
                    if ((delta.x < -30) && (aBtnShow.running === false)
                            && (delBtn.width == 0)) {
                        aBtnShow.start()
                    } else if (aBtnHide.running === false
                               && (delBtn.width > 0)) {
                        aBtnHide.start()
                    }
                }

                //鼠标选中
                onClicked: {

                    view.currentIndex = index //实现item切换

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
            return "lightblue"
        case 1:
            return "red"
        case 2:
            return "yellow"
        case 3:
            return "green"
        default:
            return "black"
        }
    }
}
