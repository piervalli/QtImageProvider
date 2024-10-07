import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    // menuBar: MenuBar {
    //     // ...
    // }

    // header: ToolBar {
    //     // ...
    // }

    // footer: TabBar {
    //     // ...
    // }
    Column {
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.bottomMargin: 5
        spacing: 5
        Image {
            id:img
            width: parent.width
            cache: true
            height: parent.height-64-parent.spacing
            fillMode: Image.PreserveAspectFit  //url of the image constains Bearer for authentication
            source:"image://async/http://127.0.0.1:8090/api/v003/product/image/019244cf-c72f-7a0c-8906-0a000d020f08?Authorization=Bearer%201234"
        }
        Row {
            width: parent.width
            Button {
                height: 64
                text:"Load"
            }
            Label {
                width: parent.height
                text:"devicePixelRatio %1 pixelDensity %2 %3x%4".arg(Screen.devicePixelRatio).arg(Screen.pixelDensity).arg(img.width).arg(img.height)
            }
        }
    }
}
