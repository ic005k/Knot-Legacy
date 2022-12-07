import QtQuick 2.0

Rectangle {
    id: root

    signal clicked()

    property alias mouseArea: mouseArea
    property alias image: image
    property int padding: 4


    implicitHeight: 30
    implicitWidth: 35
    color: mouseArea.containsMouse || mouseArea.pressed? "#4a4a4a":"transparent"// "#424242":"#4d4d4d"
    radius: 6

    Image {
        id: image
        width: parent.width-padding*2
        height: parent.height-padding*2
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
    }

    MouseArea {
        id: mouseArea

        hoverEnabled: true
        anchors.fill: parent

        onClicked: root.clicked()
    }
}
