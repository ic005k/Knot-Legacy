
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtWebView 1.1
import MyModel1 1.0

Item {
    id: mywebitem

    property bool showProgress: webView.loading && Qt.platform.os !== "ios"
                                && Qt.platform.os !== "winrt"
    visible: true

    File {
        id: file
    }

    property int m_prog: 0
    function getProg() {
        return m_prog
    }

    WebView {
        id: webView
        anchors.fill: parent
        url: initialUrl

        onLoadProgressChanged: {

            file.prog = webView.loadProgress
            m_prog = webView.loadProgress

            console.log(m_prog)

            if (webView.loadProgress == 100) {
                console.log("loadProgress=100%")
                console.log("CurrentUrl=" + webView.url)
                file.webEnd = webView.url.toString()
            }
        }
    }

    Component.onCompleted: {
        console.log(webView.url)
    }
}
