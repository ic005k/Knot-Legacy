
/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//! [Imports]
import QtQuick 2.15
import QtQuick.Window 2.15
import QtLocation 5.15
import QtPositioning 5.15

//! [Imports]
Rectangle {
    anchors.fill: parent

    // 变量定义
    property real distance: 0
    property real speed: 0
    property var previousPosition: null
    property var previousTimestamp: null

    //! [Initialize Plugin]
    Plugin {
        id: myPlugin
        name: "osm" //"osm", "mapboxgl", "esri", ...
        //specify plugin parameters if necessary
        //PluginParameter {...}
        //PluginParameter {...}
        //...

        // 指定一个可用的 OSM 瓦片服务器
        // property var tileServers: ["https://tile.opentopomap.org/{z}/{x}/{y}.png", "https://{a-c}.tile.openstreetmap.org/{z}/{x}/{y}.png", "https://stamen-tiles-{a-c}.a.ssl.fastly.net/terrain/{z}/{x}/{y}.png"]
    }
    //! [Initialize Plugin]

    //! [Current Location]
    PositionSource {
        id: positionSource
        property variant lastSearchPosition: locationOslo
        active: isGpsRun
        updateInterval: 5000 //120000 // 2 mins
        preferredPositioningMethods: PositionSource.AllPositioningMethods
        onPositionChanged: {
            var currentPosition = positionSource.position.coordinate
            map.center = currentPosition
            var distance = currentPosition.distanceTo(lastSearchPosition)
            if (distance > 500) {
                // 500m from last performed pizza search
                lastSearchPosition = currentPosition
                searchModel.searchArea = QtPositioning.circle(currentPosition)
                searchModel.update()
            }

            var newPosition = position.coordinate
            if (previousPosition) {
                // 计算两点之间的距离
                var newDistance = previousPosition.distanceTo(
                            newPosition) / 1000 // 转换为公里
                distance += newDistance

                // 计算速度
                var timeDiff = (position.timestamp - previousTimestamp) / 1000 / 3600 // 转换为小时
                if (timeDiff > 0) {
                    speed = newDistance / timeDiff
                }

                // 更新运动轨迹
                trackLine.path.push(newPosition)
            } else {
                // 初始化地图中心
                map.center = newPosition
            }

            previousPosition = newPosition
            previousTimestamp = position.timestamp
        }
    }
    //! [Current Location]

    //! [PlaceSearchModel]
    property variant locationOslo: QtPositioning.coordinate(59.93, 10.76)

    PlaceSearchModel {
        id: searchModel

        plugin: myPlugin

        searchTerm: "Pizza"
        searchArea: QtPositioning.circle(locationOslo)

        Component.onCompleted: update()
    }
    //! [PlaceSearchModel]

    //! [Places MapItemView]
    Map {
        id: map
        anchors.fill: parent
        plugin: myPlugin
        center: locationOslo
        zoomLevel: 13

        MapItemView {
            model: searchModel
            delegate: MapQuickItem {
                coordinate: place.location.coordinate

                anchorPoint.x: image.width * 0.5
                anchorPoint.y: image.height

                sourceItem: Column {
                    Image {
                        id: image
                        source: "/res/marker.png"
                    }
                    Text {
                        text: title
                        font.bold: true
                    }
                }
            }
        }

        // 运动轨迹
        MapPolyline {
            id: trackLine
            line.width: 3
            line.color: "red"
        }
    }
    //! [Places MapItemView]

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
                text: "里程: " + distance.toFixed(2) + " km"
                color: "white"
            }
            Text {
                text: "速度: " + speed.toFixed(2) + " km/h"
                color: "white"
            }
        }
    }

    Connections {
        target: searchModel
        onStatusChanged: {
            if (searchModel.status == PlaceSearchModel.Error)
                console.log(searchModel.errorString())
        }
    }
}
