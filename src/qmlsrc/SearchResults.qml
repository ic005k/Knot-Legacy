import QtQuick 2.15
import QtQuick.Controls 2.15

/*ListView {
    id: listView
    width: parent.width
    height: parent.height
    spacing: 8

    property var results: []

    function updateResults() {
        model = results
    }

    delegate: Rectangle {
        width: listView.width
        height: column.height + 16
        color: "#FFFFFF"
        radius: 6

        Column {
            id: column
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            Text {
                text: modelData.path.split("/").pop()
                font.bold: true
                font.pixelSize: 14
                color: "#212121"
            }

            Text {
                text: modelData.snippet
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                font.pixelSize: 12
                color: "#757575"
            }
        }
    }

    Connections {
        target: dbManager
        onSearchResultsReady: {
            resultModel.clear()
            results.forEach(item => resultModel.append(item))
        }
    }
}*/

ListView {
    property var results: []
    function updateResults() {
        model = results
    }

    model: ListModel { id: resultModel }
    delegate: Item {
        width: parent.width
        height: 80

        Column {
            spacing: 4
            Text {
                text: model.path.split("/").pop()
                font.bold: true
                color: "#333"
            }
            Text {
                text: model.snippet
                textFormat: Text.RichText
                wrapMode: Text.Wrap
                width: parent.width
            }
        }
    }

    Connections {
        target: dbManager
        onSearchResultsReady: {
            resultModel.clear();
            results.forEach(item => resultModel.append(item));
        }
    }
}
