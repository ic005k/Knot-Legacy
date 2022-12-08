import QtQuick 2.0

import "../components" as CustomComponents
import "../"

BaseOptionSection {
    //title: qsTr("Search text:")
    Text {
        id: lblFind
        text: qsTr("Search text:")
        color: "white"
        font.pixelSize: 16
    }

    CustomComponents.TextField {
        id: tfSearch
        width: 180
        height: 25

        textInput {
            onTextChanged: {
                pdfView.searchText(textInput.text)
            }

            font.pixelSize: 12
        }
    }


    CustomComponents.Button{
        id:btnBack
        image.source: "qrc:/icons/backward_white_24dp.svg"

        onClicked: {
            btnBack.focus=true
            pdfView.searchTextOccurance(tfSearch.textInput.text, true, false, true, true)

        }
    }
    CustomComponents.Button{
        id:btnForward
        image.source: "qrc:/icons/forward_white_24dp.svg"

        onClicked: {
            btnForward.focus=true
            pdfView.searchTextOccurance(tfSearch.textInput.text, true, false, true, false)

        }
    }



}
