import QtQuick 2.13
import QtQuick.Controls 2.13
import Qt.labs.qmlmodels 1.0

Rectangle {

    function appendTableRow(Date, Steps, KM) {

        tableModel.appendRow({
                                 "Date": Date,
                                 "Steps": Steps,
                                 "KM": KM
                             })
    }

    function setScrollBarPos(pos) {
        tableView.contentY = 0 //tableView.contentHeight - tableView.height
        console.log("contentH=" + tableView.contentHeight + "  h=" + tableView.height)
    }

    function getItemCount() {

        return tableModel.rowCount
    }

    function getDate(itemIndex) {
        var data = tableModel.getRow(itemIndex)
        return data.Date
    }

    function getSteps(itemIndex) {
        var data = tableModel.getRow(itemIndex)
        return data.Steps
    }

    function getKM(itemIndex) {
        var data = tableModel.getRow(itemIndex)
        return data.KM
    }

    function setTableData(currentIndex, date, steps, km) {
        tableModel.setRow(currentIndex, {
                              "Date": date,
                              "Steps": steps,
                              "KM": km
                          })
    }

    function delItem(currentIndex) {
        tableModel.removeRow(currentIndex)
    }

    property var header1: [qsTr("Time"), qsTr("Amount"), qsTr("Category")]
    property var header2: [qsTr("Date"), qsTr("Time"), qsTr("Amount")]
    property var cur_header: [qsTr("Time"), qsTr("Amount"), qsTr("Category")]
    function setHeader(sn) {

        if (sn === 1)
            cur_header = header1
        if (sn === 2)
            cur_header = header2
    }

    Rectangle {
        id: header
        width: parent.width
        height: 30

        Row {
            spacing: 0

            Repeater {
                // Table Header
                model: cur_header

                Rectangle {
                    width: header.width / 3
                    height: header.height
                    color: "#666666"
                    border.width: 1
                    border.color: "#848484"
                    Text {
                        text: modelData
                        anchors.centerIn: parent
                        color: "white"
                    }
                }
            }
        }
    }
    TableView {
        id: tableView

        width: parent.width
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        boundsBehavior: Flickable.OvershootBounds

        ScrollBar.vertical: ScrollBar {
            id: vbar
            anchors.right: parent.right
            anchors.rightMargin: 0
            policy: ScrollBar.AsNeeded
            width: 8
            visible: tableModel.rowCount > 5
            background: Rectangle {
                color: "#666666"
            }
        }

        model: TableModel {
            id: tableModel

            TableModelColumn {
                display: "Date"
            }
            TableModelColumn {
                display: "Steps"
            }
            TableModelColumn {
                display: "KM"
            }
        }
        delegate: Rectangle {
            color: "#666666"
            implicitWidth: tableView.width / 3
            implicitHeight: 32
            border.width: 1
            border.color: "#848484"

            Text {
                text: display
                anchors.centerIn: parent

                color: "white"
            }
        }
    }

    Component.onCompleted: {
        appendTableRow("2022-11-19", "3500", "4567")
    }
}
