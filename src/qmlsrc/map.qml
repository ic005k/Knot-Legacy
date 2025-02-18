import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtLocation 5.15
import QtPositioning 5.15

Rectangle {
    visible: true
    width: 800
    height: 600

    // 变量定义
    property double gpsx: 59.91
    property double gpsy: 10.75

    function appendTrack(lat, lon) {
        gpsx = lat
        gpsy = lon

        var newCoordinate = QtPositioning.coordinate(lat, lon)

        //polyline.path.push([newCoordinate])
        let pathArray = polyline.path

        pathArray.push(newCoordinate) // 添加新的点
        polyline.path = pathArray

        map.center = newCoordinate
    }

    function clearTrack() {
        polyline.path = []
    }

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(gpsx, gpsy) // 初始中心坐标（伦敦）
        zoomLevel: 13

        MapPolyline {
            id: polyline
            line.color: "red"
            line.width: 3

            // test
            // path: [QtPositioning.coordinate(59.91,
            //                                10.75),
            //    QtPositioning.coordinate(
            //         gpsx+0.0000, gpsy+0.0000)//QtPositioning.coordinate(59.912, 10.752)
            // ]
        }

        // 可选：添加一个标记来表示当前位置
        MapQuickItem {
            coordinate: QtPositioning.coordinate(gpsx, gpsy)
            anchorPoint.x: markerImage.width / 2 + 6
            anchorPoint.y: markerImage.height / 2
            sourceItem: Image {
                id: markerImage
                source: "/res/marker.png" // 替换为你的标记图标
                width: 32
                height: 32
            }
        }
    }

    PositionSource {
        id: positionSource
        active: false

        onPositionChanged: {
            var coord = positionSource.position.coordinate
            if (coord.isValid) {
                // 更新地图中心
                map.center = coord

                // 添加新坐标到轨迹
                trajectory.addCoordinate(coord)
            }
        }
    }

    // 运动信息显示区域
    Rectangle {
        id: infoArea
        anchors {
            left: parent.left
            bottom: parent.bottom
            leftMargin: 0
            bottomMargin: 0
        }
        width: parent.width
        height: 35
        color: "blue"
        radius: 0

        // Column {
        Row {
            anchors.centerIn: parent
            spacing: 5
            Text {
                text: qsTr("Distance") + ": " + strDistance
                color: "white"
            }
            Text {
                text: qsTr("Speed") + ": " + strSpeed
                color: "white"
            }
        }
    }
}
