import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebView 1.1

Item {
    id: mywebitem

    property string pdfpath: ""

    function setPdfPath(pdffile)
    {
        pdfpath = pdffile
        console.debug("pdfpath=" + pdfpath)
    }

    property bool showProgress: webView.loading && Qt.platform.os !== "ios"
                                && Qt.platform.os !== "winrt"
    visible: true

    property int m_prog: 0
    function getProg() {
        return m_prog
    }

    WebView {
        id: webView
        anchors.fill: parent

        url: pdfpath
        onLoadProgressChanged: {

        }
    }

    Component.onCompleted: {
        WebView.url = pdfpath
        console.log(webView.url)
    }
}
