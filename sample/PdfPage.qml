import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtWebView 1.1

import "components" as CustomComponents
import "app"
import "app/permissions"

import it.ltdev.qt.qml.components 1.0 as LTDev

Rectangle {
    id: root
    width: 500
    height: 600
    visible: true
    color: "#3e3f3f"

    property StackView view
    property string pdfPath: ""
    property bool isViewEnd: false

    function setViewVisible(vv) {
        pdfView.visible = vv
    }

    function loadPDF(pdffile) {
        pdfPath = pdffile

        if (isViewEnd) {
            pdfView.visible = true
            pdfView.load("")
            pdfView.load(pdfPath)
        }

        console.debug("pdfFile is open ...... " + pdfPath + "   " + isViewEnd)
    }

    property int closedTopbarHeight: 42
    property int topbarHeight: {
        var h = closedTopbarHeight
        var optionsHeight = containerOptions.visible ? divider.height + containerOptions.height : 0
        var pagesPreviewHeight = containerPreviewPages.visible ? divider2.height
                                                                 + containerPreviewPages.height : 0

        return closedTopbarHeight + optionsHeight + pagesPreviewHeight
    }

    Column {
        anchors.fill: parent
        spacing: 0

        Item {
            id: topbar
            width: parent.width
            height: topbarHeight

            Behavior on height {
                SmoothedAnimation {
                    id: animOpen
                    duration: 300
                    easing.type: Easing.InOutCubic
                }
            }

            Column {
                id: columnLayout
                width: parent.width - 36
                height: parent.height - 6
                anchors.centerIn: parent

                RowLayout {
                    id: header
                    spacing: 2
                    width: parent.width

                    Text {
                        text: qsTr("PDF")
                        font.pixelSize: 16
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                    }

                    Row {
                        spacing: 2

                        CustomComponents.Button {
                            id: btnBack
                            visible: true
                            padding: 4
                            image.source: "qrc:/icons/back.svg"

                            onClicked: {
                                mw_one.on_btnBack_clicked()
                            }
                        }

                        CustomComponents.Button {
                            id: btnOpen
                            visible: true
                            padding: 4
                            image.source: "qrc:/icons/open.svg"

                            onClicked: {
                                mw_one.on_btnOpen_clicked()
                            }
                        }

                        CustomComponents.Button {
                            id: btnBookList
                            visible: true
                            padding: 4
                            image.source: "qrc:/icons/booklist.svg"

                            onClicked: {
                                mw_one.on_btnReadList_clicked()
                            }
                        }

                        CustomComponents.Button {
                            id: btnShowPreview
                            property bool previewEnabled: false
                            visible: pdfView.thumbnails.length > 0

                            padding: 4
                            color: previewEnabled ? "#4d4d4d" : "transparent"
                            image.source: "qrc:/icons/table_chart_white_24dp.svg"

                            onClicked: {
                                previewEnabled = !previewEnabled
                            }
                        }

                        CustomComponents.Button {
                            id: btnOptions
                            property bool optionsEnabled: false

                            padding: 4
                            color: optionsEnabled ? "#4d4d4d" : "transparent"
                            image.source: "qrc:/icons/tune_white_24dp.svg"

                            onClicked: {
                                optionsEnabled = !optionsEnabled
                            }
                        }
                    }
                } // RowLayout

                Item {
                    id: divider
                    visible: containerOptions.visible
                    width: parent.width
                    height: 24
                }


                /*
                    PDF Options
                */
                Flow {
                    id: containerOptions
                    visible: btnOptions.optionsEnabled
                    spacing: 24

                    width: parent.width

                    OptionPageScrollMode {
                        pdfView: pdfView
                    }

                    OptionPageSelection {
                        pdfView: pdfView
                    }

                    OptionPageRotation {
                        pdfView: pdfView
                    }

                    OptionPageSearch {
                        pdfView: pdfView
                    }

                    OptionPageScaling {
                        pdfView: pdfView
                        maxWidth: parent.width
                    }
                }

                Item {
                    id: divider2
                    visible: containerPreviewPages.visible
                    width: parent.width
                    height: 24
                }


                /*
                    PDF pages preview
                */
                Item {
                    id: containerPreviewPages
                    visible: btnShowPreview.previewEnabled
                    width: parent.width
                    height: previewPages.height

                    PreviewPages {
                        id: previewPages
                        pdfView: pdfView

                        width: parent.width
                        anchors.centerIn: parent
                    }
                }
            }
        } // Top bar

        Rectangle {
            id: pdfContainer
            width: parent.width
            height: parent.height - closedTopbarHeight
            color: "lightgrey"

            LTDev.PdfView {
                id: pdfView
                anchors.fill: parent

                // Setting visibility/opacity to manage loading states (eg. showing an error message or a busy indicator):
                // - on Desktop: setting the opacity (only) should be sufficient to managing loading states
                // - on Android: must be set both opacity and visibility to managing loading states
                visible: false
                opacity: 0

                onError: {
                    // Hide pdfview on error
                    pdfView.visible = false
                    pdfView.opacity = 0

                    console.error("Error: ", message)

                    // Parse json error message
                    var json = JSON.parse(message)

                    // Update container error text
                    containerError.textView.text = "Error: "
                            + json.error.generic + " " + json.error.detailed
                }

                onViewerLoaded: {
                    isViewEnd = true
                    pdfView.load(pdfPath)
                    pdfView.visible = true

                    console.debug("onViewerLoaded......")
                }

                onPdfLoaded: {
                    // Pdf has been correctly loaded, ensure pdf view visibility
                    pdfView.visible = true
                    pdfView.opacity = 1

                    // Update container error text (no error occurred)
                    containerError.textView.text = ""

                    console.debug("onPdfLoaded......")
                }
            }


            /*
                Error info
            */
            CustomComponents.ContainerInfo {
                id: containerError
                visible: !pdfView.visible && textView.text.length > 0

                width: Math.min(parent.width / 2, 400)
                height: 65
                anchors.centerIn: parent
            }
        }
    }
}
