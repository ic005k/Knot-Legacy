import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

import it.ltdev.qt.qml.components 1.0 as LTDev

Item {
    id: pdfitem
    visible: true


    property bool isViewEnd: false
    property string pdfPath: ""

    function loadPDF(pdffile) {
        pdfPath = pdffile

        if (isViewEnd){
            pdfView.load(pdfPath)
        }

        console.debug("pdfFile is open ...... " + pdfPath + "   " + isViewEnd)
    }

    LTDev.PdfView {
        id: pdfView
        anchors.fill: parent

        // Setting visibility/opacity to manage loading states (eg. showing an error message or a busy indicator):
        // - on Desktop: setting the opacity (only) should be sufficient to managing loading states
        // - on Android: must be set both opacity and visibility to managing loading states
        visible: true
        opacity: 0

        onError: {
            // Hide pdfview on error
            //pdfView.visible = false
            //pdfView.opacity = 0

            console.error("Error: ", message)

            // Parse json error message
            var json = JSON.parse(message)

            // Update container error text
            //containerError.textView.text = "Error: " + json.error.generic + " "
            //        + json.error.detailed
        }

        onViewerLoaded: {

            isViewEnd = true

            pdfView.load(pdfPath)
            pdfView.visible = true
            console.debug("onViewerLoaded......  " + isViewEnd)
        }

        onPdfLoaded: {
            // Pdf has been correctly loaded, ensure pdf view visibility
            pdfView.visible = true
            pdfView.opacity = 1
            console.debug("onPdfLoaded......")
        }
    }


}
