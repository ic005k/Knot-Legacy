import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ListView {
    id: listView
    anchors.fill: parent
    clip: true
    spacing: 8
    model: searchResultModel

    delegate: ItemDelegate {
        width: listView.width
        height: 80
        hoverEnabled: true

        // 背景悬停效果
        background: Rectangle {
            color: hovered ? "#f5f6fa" : "transparent"
            radius: 4
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 4

            // 文件标题
            Text {
                text: filePath.split("/").pop()
                font.pixelSize: 16
                color: "#2c3e50"
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            // 高亮预览文本
            Text {
                text: formatPreviewText(previewText, highlightPos)
                font.pixelSize: 14
                color: "#7f8c8d"
                wrapMode: Text.Wrap
                maximumLineCount: 2
                Layout.fillWidth: true
                textFormat: Text.RichText // 启用富文本
            }
        }

        // 高亮格式化函数
        function formatPreviewText(previewText, highlightPositions) {
            let result = previewText
            // 深拷贝并反向排序（避免插入标签导致偏移）
            let sortedPositions = highlightPositions.slice().sort((a, b) => b.charStart - a.charStart)

            sortedPositions.forEach(pos => {
                const start = pos.charStart
                const end = pos.charEnd
                if (start >= 0 && end <= result.length) {
                    const keyword = result.slice(start, end)
                    result = result.slice(0, start) +
                             `<span style="color: #e74c3c; font-weight: 500;">${keyword}</span>` +
                             result.slice(end)
                }
            })
            return result
        }

        // 点击打开文件
        onClicked: console.log("Open file:", filePath)
    }

    // 空状态提示
    Label {
        anchors.centerIn: parent
        visible: listView.count === 0
        text: "未找到相关结果"
        color: "#bdc3c7"
        font.pixelSize: 18
    }

    // 滚动条
    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
        width: 8
    }
}
