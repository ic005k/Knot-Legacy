import QtQuick 2.15
import QtQuick.Window 2.15
import QtLocation 5.15
import QtPositioning 5.15

Rectangle {
    visible: true
    width: 800
    height: 600

    // 变量定义
    property double gpsx: 51.5074
    property double gpsy: -0.1278

    //property string strSpeed: "0.00"
    // property string strDistance: "0.00"
    function appendTrack(lat, lon) {

        // trajectory.path.push({
        //                          "latitude": lat,
        //                          "longitude": lon
        //                      })
        var newPath = [{
                           "latitude": lat,
                           "longitude": lon
                       } // London
                       // Paris
                ]
        trajectory.path = newPath // 使用新的路径更新
        gpsx = lat
        gpsy = lon
        map.center = QtPositioning.coordinate(gpsx, gpsy)
    }

    function clearTrack() {
        trajectory.path = []
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
        zoomLevel: 14

        MapPolyline {
            id: trajectory
            line.color: "red"
            line.width: 3
            path: [// 初始轨迹点
                QtPositioning.coordinate(51.5074,
                                         -0.1278), QtPositioning.coordinate(
                    51.5080, -0.1280), QtPositioning.coordinate(
                    51.507, -0.1279), QtPositioning.coordinate(51.5089,
                                                               -0.1289)]
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
