import QtQuick 2.13
import QtQuick.Controls 2.13
import Qt.labs.qmlmodels 1.0

Rectangle {


    Rectangle{
        id:header
        width: parent.width
        height: 30

        Row{
            spacing: 0

            Repeater{
                model: [qsTr("Date"),qsTr("Steps"),qsTr("KM")]

                Rectangle{
                    width: header.width/4
                    height: header.height
                    color: "#666666"
                    border.width: 1
                    border.color: "#848484"
                    Text {
                        text: modelData
                        anchors.centerIn: parent
                        font.pointSize: 12
                        color: "white"
                    }
                }
            }
        }
    }
    TableView{
        id:tableView
        width: parent.width
        anchors.top:header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        boundsBehavior: Flickable.OvershootBounds


        ScrollBar.vertical: ScrollBar {
            anchors.right:parent.right
            anchors.rightMargin: 0
            visible: tableModel.rowCount > 5
            background: Rectangle{
                color:"#666666"
            }
            onActiveChanged: {
                active = true;
            }
            contentItem: Rectangle
            {
                implicitWidth  : 6
                implicitHeight : 30
                radius : 3
                color  : "#848484"
            }
        }

        model: TableModel {
            id:tableModel

            TableModelColumn{display: qsTr("Date")}
            TableModelColumn{display: qsTr("Steps")}
            TableModelColumn{display: qsTr("KM")}


        }
        delegate:Rectangle{
            color: "#666666"
            implicitWidth: tableView.width/4
            implicitHeight: 32
            border.width: 1
            border.color: "#848484"

            Text {
                text: display
                anchors.centerIn: parent
                font.pointSize: 12
                color: "white"
            }
        }
    }

    Component.onCompleted: {
        //tableModel.appendRow({"Date":"小明","Steps":12,"KM":"三年二班"})

    }

    function appendTableRow(Date,Steps,KM){

    tableModel.appendRow({"Date":Date,"Steps":Steps,"KM":KM})
}

}

