import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

Rectangle {

    function appendTableRow(Date, Steps, KM) {

        tableModel.appendRow({
                                 "Date": Date,
                                 "Freq": Steps,
                                 "Amount": KM
                             })
    }

    function setScrollBarPos(pos) {

        tableView.contentY = 0 //tableView.contentHeight - tableView.height
        console.log("contentH=" + tableView.contentHeight + "  h=" + tableView.height)
    }

    function getCurrentIndex() {
        return tableView.currentIndex
    }

    function getItemCount() {

        return tableModel.rowCount
    }

    function getDate(itemIndex) {
        return tableModel.rows[itemIndex].Date
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
                              "Freq": steps,
                              "Amount": km
                          })
    }

    function delItem(currentIndex) {
        tableModel.removeRow(currentIndex)
    }

    Rectangle {
        id: header
        width: parent.width
        height: 30

        Row {
            spacing: 0

            Repeater {
                // Table Header
                model: [qsTr("Date"), qsTr("Freq"), qsTr("Amount")]

                Rectangle {
                    width: header.width / 3
                    height: header.height
                    color: "#666666"
                    border.width: 1
                    border.color: "#848484"
                    Text {
                        text: modelData
                        anchors.centerIn: parent
                        //font.pointSize: 12
                        color: "white"
                    }
                }
            }
        }
    }
    TableView {
        property int hoverIndex: -1
        property int currentIndex: -1
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
                display: "Freq"
            }
            TableModelColumn {
                display: "Amount"
            }
        }
        delegate: Rectangle {
            //color: "#666666"
            color: {
                //tableView.currentIndex === row ? "#3298FE" : (tableView.hoverIndex === row ? "#97CBFF" : (row % 2 ? "#666666" : "#666666"))
                tableView.currentIndex === row ? "#3298FE" : (row % 2 ? "#666666" : "#666666")
            }

            implicitWidth: tableView.width / 3
            implicitHeight: 32
            border.width: 1
            border.color: "#848484"

            Text {
                text: display
                anchors.centerIn: parent

                color: "white"
            }

            MouseArea {
                id: cashierMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    tableView.forceActiveFocus()
                    tableView.currentIndex = row

                    mydlgReport.loadDetails()

                    console.debug(row)
                    //console.log(tableModel.rows[row].Date)
                }
                onDoubleClicked: {
                    tableView.selected(row, header.logicIndexMap[column])
                }
                onEntered: tableView.hoverIndex = row
                onExited: tableView.hoverIndex = -1
            }
        }
    }

    Component.onCompleted: {
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
        appendTableRow("2022-11-19", "3500", "test1")
    }
}
