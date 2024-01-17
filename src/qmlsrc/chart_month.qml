import QtQuick 2.15
import QtQuick.Window 2.15
import QtQml 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

Rectangle {
    id: root

    width: 500
    height: 400

    color: isDark ? "#19232D" : "white"

    Item {
        anchors.fill: parent

        //![1]
        ChartView {
            title: "Two Series, Common Axes"
            anchors.fill: parent
            legend.visible: false
            antialiasing: true

            ValueAxis {
                id: axisX
                min: 1
                max: 31
                tickCount: 1
            }

            ValueAxis {
                id: axisY
                min: 0
                max: 50
            }

            BarSeries {
                id: series1

                //axisX: axisX
                //axisY: axisY
                axisX: BarCategoryAxis {
                    categories: ["2007", "2008", "2009", "2010", "2011", "2012"]
                }
                BarSet {
                    label: "Bob"
                    values: [2, 2, 3, 4, 5, 6]
                }
                BarSet {
                    label: "Susan"
                    values: [5, 1, 2, 4, 1, 7]
                }
            }

            ScatterSeries {
                id: series2
                axisX: axisX
                axisY: axisY
            }
        }

        // Add data dynamically to the series
        Component.onCompleted: {
            for (var i = 1; i <= 31; i++) {

                //series1.append(i, Math.random())
                //series2.append(i, Math.random())
            }
        }
        //![1]
    }

    ListView {
        id: m_View
        focus: true
        anchors.fill: parent
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightMoveDuration: 250
        orientation: ListView.Horizontal
        boundsBehavior: Flickable.StopAtBounds

        onCurrentIndexChanged: {

        }

        model: ListModel {
            ListElement {
                component: "View1.qml"
            }
            ListElement {
                component: "View2.qml"
            }
            ListElement {
                component: "View3.qml"
            }
        }

        delegate: Loader {
            width: root.width
            height: root.height

            source: component
            asynchronous: true

            onLoaded: sourceLoaded = true
        }
    }
}
