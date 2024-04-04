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
    property bool isFunpanel: true
    property bool isFind: false
    property bool isShowPanel: false
    property int padd: 2
    property int currentPage: 1
    property real currentScale: 1
    property bool isOne: true
    property bool isHeaderVisible: true

    function goWebViewBack()
    {
        pdfView.webView.goBack();
    }

    function getLoadProgress() {
        return pdfView.webView.loadProgress
    }

    function setHideShowTopBar() {
        if (isHeaderVisible) {
            closedTopbarHeight = 0
            isHeaderVisible = false
        } else if (!isHeaderVisible) {
            closedTopbarHeight = 42
            isHeaderVisible = true
        }
    }

    function getCurrentPage() {
        return pdfView.page
    }

    function getScale() {
        return pdfView.scale
    }

    function setPdfPage(_page) {
        isOne = false
        currentPage = _page
    }

    function setPdfScale(_scale) {
        isOne = false
        currentScale = _scale
    }

    function setViewVisible(vv) {
        if (vv === true)
            pdfView.opacity = 1
        if (vv === false)
            pdfView.opacity = 0

        pdfView.visible = vv
    }

    function setViewEnd(vv) {
        isViewEnd = vv
    }

    function loadPDFWin(pdfjs, pdfpath) {
        pdfView.visible = true
        pdfView.opacity = 1
        pdfView.webView.url = pdfjs + encodeURIComponent(pdfpath)

        console.debug(
                    "pdfFile is open ...... " + pdfView.webView.url + "  isViewEnd=" + isViewEnd)
    }

    function loadPDF(pdffile) {
        pdfPath = pdffile
        isOne = false
        console.log("isViewEnd=" + isViewEnd)

        if (isViewEnd) {
            pdfView.visible = true
            pdfView.opacity = 1
            pdfView.load(pdfPath)
            console.debug("======pdfFile is open: " + pdfView.webView.url
                          + "  isViewEnd=" + isViewEnd)
        }
    }

    function rotatePage() {
        pdfView.rotate(-90)
        //pdfView.setScrollMode(pdfView._SCROLL_MODE_HORIZONTAL)
        pdfView.setViewerModes(pdfView._SCROLL_MODE_HORIZONTAL, 0)
    }

    property int closedTopbarHeight: 0 //30
    property int topbarHeight: {
        var h = closedTopbarHeight
        var optionsHeight = containerOptions.visible ? divider.height + containerOptions.height : 0
        var pagesPreviewHeight = containerPreviewPages.visible ? divider2.height
                                                                 + containerPreviewPages.height : 0

        return closedTopbarHeight + optionsHeight + pagesPreviewHeight
    }

    Column {
        id: col0
        anchors.fill: parent
        spacing: 0

        Item {
            id: topbar
            width: parent.width
            height: topbarHeight
            visible: false // isHeaderVisible

            Behavior on height {
                SmoothedAnimation {
                    id: animOpen
                    duration: 300
                    easing.type: Easing.InOutCubic
                }
            }

            Column {
                id: columnLayout
                width: parent.width - 2
                height: parent.height - 0
                anchors.centerIn: parent

                RowLayout {
                    id: header

                    spacing: 2
                    width: parent.width

                    Text {
                        visible: true
                        text: qsTr("P\nD\nF")
                        font.pixelSize: 6
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                        anchors.leftMargin: 2
                    }

                    Row {
                        spacing: 10

                        CustomComponents.Button {
                            id: btnBack
                            visible: true
                            padding: padd
                            image.source: "qrc:/icons/back.svg"

                            onClicked: {
                                mw_one.on_btnBackReader_clicked()
                            }
                        }

                        CustomComponents.Button {
                            id: btnRotate
                            visible: true
                            padding: padd
                            image.source: "qrc:/icons/r.png"

                            onClicked: {
                                rotatePage()
                            }
                        }

                        CustomComponents.Button {
                            id: btnGoBack
                            visible: true
                            padding: padd
                            image.source: "qrc:/icons/gb.svg"

                            onClicked: {
                                pdfView.webView.goBack()
                            }
                        }

                        CustomComponents.Button {
                            id: btnGoForward
                            visible: true
                            padding: padd
                            image.source: "qrc:/icons/gf.svg"

                            onClicked: {
                                pdfView.webView.goForward()
                            }
                        }

                        CustomComponents.Button {
                            id: btnSmall
                            visible: false
                            padding: padd
                            image.source: "qrc:/icons/remove_white_24dp.svg"

                            onClicked: {
                                pdfView.zoomOut()
                                console.debug("zoomOut " + pdfView.scale)
                            }
                        }

                        CustomComponents.Button {
                            id: btnBig
                            visible: false
                            padding: padd
                            image.source: "qrc:/icons/add_white_24dp.svg"

                            onClicked: {
                                pdfView.zoomIn()
                                console.debug("zoomIn " + pdfView.scale)
                            }
                        }

                        CustomComponents.Button {
                            id: btnFind
                            visible: false
                            property bool optionsEnabled: false

                            padding: padd
                            color: optionsEnabled ? "#4d4d4d" : "transparent"
                            image.source: "qrc:/icons/find.svg"

                            onClicked: {
                                isFunpanel = false
                                isFind = true
                                optionsEnabled = !optionsEnabled
                                isShowPanel = optionsEnabled
                            }
                        }

                        CustomComponents.Button {
                            id: btnOpen
                            visible: true
                            padding: padd
                            image.source: "qrc:/icons/open.svg"

                            onClicked: {
                                mw_one.on_btnOpen_clicked()
                            }
                        }

                        CustomComponents.Button {
                            id: btnBookList
                            visible: true
                            padding: padd
                            image.source: "qrc:/icons/booklist.png"

                            onClicked: {
                                mw_one.on_btnReadList_clicked()
                            }
                        }

                        CustomComponents.Button {
                            id: btnShowPreview
                            property bool previewEnabled: false
                            visible: pdfView.thumbnails.length > 0

                            padding: padd
                            color: previewEnabled ? "#4d4d4d" : "transparent"
                            image.source: "qrc:/icons/list.png"

                            onClicked: {

                                previewEnabled = !previewEnabled
                            }
                        }

                        CustomComponents.Button {
                            id: btnOptions
                            property bool optionsEnabled: false

                            padding: padd
                            color: optionsEnabled ? "#4d4d4d" : "transparent"
                            image.source: "qrc:/icons/tune_white_24dp.svg"

                            onClicked: {
                                isFind = false
                                isFunpanel = true
                                optionsEnabled = !optionsEnabled
                                isShowPanel = optionsEnabled
                            }
                        }
                    }
                } // RowLayout

                Item {
                    id: divider
                    visible: containerOptions.visible
                    width: parent.width
                    height: 5
                }


                /*
                    PDF Options
                */
                Flow {
                    id: containerOptions
                    visible: isShowPanel //btnOptions.optionsEnabled
                    spacing: 6

                    width: parent.width

                    OptionPageScrollMode {
                        visible: isFunpanel
                        pdfView: pdfView
                    }

                    OptionPageSelection {
                        visible: isFunpanel
                        pdfView: pdfView
                    }

                    OptionPageRotation {
                        visible: isFunpanel
                        pdfView: pdfView
                    }

                    OptionPageSearch {
                        visible: isFind
                        pdfView: pdfView
                    }

                    OptionPageScaling {
                        visible: isFunpanel
                        pdfView: pdfView
                        maxWidth: parent.width
                    }
                }

                Item {
                    id: divider2
                    visible: containerPreviewPages.visible
                    width: parent.width
                    height: 5
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
            height: (parent.height - closedTopbarHeight)
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
                    pdfView.visible = true
                    pdfView.opacity = 1

                    console.error("Error: ", message)

                    // Parse json error message
                    var json = JSON.parse(message)

                    // Update container error text
                    containerError.textView.text = "Error: "
                            + json.error.generic + " " + json.error.detailed
                }

                onViewerLoaded: {

                    pdfView.visible = true
                    pdfView.opacity = 1
                    pdfView.load(pdfPath)
                    isViewEnd = true

                    console.debug("===onViewerLoaded= ")
                }

                onPdfLoaded: {
                    if (!isOne) {
                        isOne = true
                        pdfView.visible = true
                        pdfView.opacity = 1

                        pdfView.scale = currentScale
                        pdfView.setScale(currentScale)
                        pdfView.setPage(currentPage)
                    }

                    console.debug(
                                "===onPdfLoaded=  " + currentPage + "  "
                                + currentScale + " " + pdfView.webView.loadProgress)
                    mw_one.closeProgress()
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
