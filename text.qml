import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.12
import MyModel 1.0
//import qmlText 1.0



Item {
    visible: true
    width: 400
    height: 400

    File {
        id: file
        source: "" //"/Users/hz/Documents/1.txt"
    }

    Image {
            id: rocket
            fillMode: Image.TileHorizontally
            smooth: true
            source: "/src/b.png"
        }

    Flickable {
            id: flickable
            flickableDirection: Flickable.VerticalFlick
            anchors.fill: parent



            TextArea.flickable: TextArea {
                id: textArea
                anchors.fill: parent
                font.pixelSize: 18
                textFormat: Qt.PlainText


                //onTextChanged: file.text = text
                Component.onCompleted: text = file.text

               // background: "qrc:/b.png"
                wrapMode: TextArea.Wrap
                readOnly: true
                color: "#000000"
                background: rocket


                /*text: "<p style='line-height:28px; width:100% ; white-space: pre-wrap; '>" +
                      "第二次世界大战和战后世界政治格局的巨大变化，极大地冲击和震撼了资本主义各国的社会，在这个背景下，结构功能主义学派应运而生。战后，它首先在美国崛起，一直持续至60年代。它着重从功能上分析研究社会系统的制度性结构。结构功能主义的代表人物是美国社会学家塔尔科特·帕森斯（Talcot　Parsons，1902-1979年）和罗伯特·金·麦敦（Robert King　　 Merton）。与社会冲突论相反，结构功能主义持保守的社会观点，它注重研究社会运行和社会发展中的平衡，主张社会的稳定和整合，代表社会的保守派。
　　可以说，社会冲突论是对结构功能主义的反思和对立物。不过，二者都是维护型的社会学，即一个企图通过保持社会稳定，一个更多主张社会改革，巩固和保持社会制度，首先是巩固和维护资本主义的社会制度。如果回忆一下60年代美欧各国的社会运动、尤其是学生运动风起云涌，锐不可挡，那么，社会冲突论在美欧的出现就并非偶然了。同样，如果人们看一看达仁道夫在60年代积极主张大学改革、社会改革，那么，他成为社会冲突论在欧洲的代表也是在情理之中的。    " + "</p>";*/
            }

            ScrollBar.vertical: ScrollBar {
                width: 6
                active: true
                //policy: ScrollBar.AlwaysOn
            }

            property int rowHeight: textArea.font.pointSize+20
            property int marginsTop: 10
            property int marginsLeft: 4
            property int lineCountWidth: 40
        }





}
