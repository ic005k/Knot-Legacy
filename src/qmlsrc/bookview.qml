import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    width: 360
    height: 640
    visible: true
    title: "电子书阅读器"

    Item {
        id: root
        anchors.fill: parent

        // 配置属性
        property int currentIndex: 0
        property var files: generateTestContent(5)  // 生成5页测试内容
        property string currentContent: "正在加载..."
        property bool animating: false
        property string nextContent: ""

        // 生成测试内容
        function generateTestContent(pageCount) {
            let testPages = []
            for(let i = 0; i < pageCount; i++) {
                const content = `第${i+1}页\n测试内容 ${i+1}\n滑动查看下一页→`
                testPages.push(`data:text/plain;charset=utf-8,${encodeURIComponent(content)}`)
            }
            return testPages
        }

        // 初始化加载
        Component.onCompleted: loadFile(currentIndex)

        // 当前页面视图
        Rectangle {
            id: currentPage
            anchors.fill: parent
            color: "#f5f5f5"
            visible: !root.animating

            ScrollView {
                anchors.fill: parent
                padding: 15

                TextArea {
                    text: root.currentContent
                    wrapMode: Text.Wrap
                    readOnly: true
                    background: Rectangle { color: "transparent" }
                }
            }
        }

        // 翻页容器（使用2D变换实现翻页效果）
        Item {
            id: swipeContainer
            anchors.fill: parent
            visible: root.animating
            clip: true

            Rectangle {
                id: targetPage
                width: parent.width
                height: parent.height
                color: "#f5f5f5"
                transform: [
                    Rotation {
                        id: pageRotation
                        origin.x: 0
                        axis { x: 0; y: 1; z: 0 }  // 设置Y轴旋转
                        angle: 0
                    },
                    Translate {
                        id: pageTranslate
                        x: 0
                    }
                ]

                ScrollView {
                    anchors.fill: parent
                    padding: 15

                    TextArea {
                        text: root.nextContent
                        wrapMode: Text.Wrap
                        readOnly: true
                        background: Rectangle { color: "transparent" }
                    }
                }
            }
        }

        // 手势检测
        MouseArea {
            anchors.fill: parent
            preventStealing: true
            property real startX: 0

            onPressed: startX = mouse.x
            onReleased: handleSwipe(mouse.x)

            function handleSwipe(endX) {
                if(root.animating) return

                const delta = endX - startX
                if(Math.abs(delta) > 50) {
                    const direction = delta > 0 ? -1 : 1
                    if(canSwipe(direction)) {
                        startSwipe(direction)
                    }
                }
            }

            function canSwipe(dir) {
                return (dir === -1 && root.currentIndex > 0) ||
                       (dir === 1 && root.currentIndex < root.files.length-1)
            }

            function startSwipe(dir) {
                root.animating = true
                const targetIndex = root.currentIndex + dir
                root.loadFileAsync(targetIndex, content => {
                    root.nextContent = content
                    pageAnimation.direction = dir
                    pageAnimation.start()
                })
            }
        }

        // 翻页动画（修正版）
        ParallelAnimation {
            id: pageAnimation
            property int direction: 1

            NumberAnimation {
                target: pageTranslate
                property: "x"
                from: 0
                to: direction * -swipeContainer.width
                duration: 600
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: pageRotation
                property: "angle"
                from: 0
                to: direction * 180
                duration: 600
                easing.type: Easing.InOutQuad
            }

            onFinished: {
                root.currentIndex += direction
                root.currentContent = root.nextContent
                root.animating = false
                pageTranslate.x = 0
                pageRotation.angle = 0
            }
        }

        // 文件加载方法
        function loadFile(index) {
            const xhr = new XMLHttpRequest()
            xhr.open("GET", root.files[index])
            xhr.onreadystatechange = () => {
                if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
                    root.currentContent = decodeURIComponent(xhr.responseText)
                }
            }
            xhr.send()
        }

        function loadFileAsync(index, callback) {
            const xhr = new XMLHttpRequest()
            xhr.open("GET", root.files[index])
            xhr.onreadystatechange = () => {
                if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
                    callback(decodeURIComponent(xhr.responseText))
                }
            }
            xhr.send()
        }

        // 分页指示器
        PageIndicator {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 20
            }
            count: root.files.length
            currentIndex: root.currentIndex
            delegate: Rectangle {
                width: 12
                height: 12
                radius: 6
                color: index === root.currentIndex ? "#2196F3" : "#90CAF9"
                opacity: root.animating ? 0.3 : 1.0
                Behavior on opacity { NumberAnimation { duration: 200 } }
            }
        }
    }
}
