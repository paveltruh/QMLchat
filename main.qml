import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Messages 1.0

Window {
    id: window
    visible: true
    width: 640
    height: 480
    title: qsTr("Chat")
    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: LoginPage {}
    }

    function join(name) {
        chatModel.joinToChat(name);
        stackView.push("ConversationPage.qml");
    }

    MessagesModel{
        id: chatModel
//        onFileOpened: {
//            fileLable.text = fileName
//        }
    }
}
