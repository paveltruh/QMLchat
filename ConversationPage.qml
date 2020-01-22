import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
//import QtQuick.Dialogs 1.2
import QtQml 2.12
import Qt.labs.platform 1.1

import Messages 1.0
Page {
    anchors.fill: parent
    FileDialog{
        id: openFileDialog
        title: "Open file"
        onAccepted: {
            fileLable.text = chatModel.openFile(file);
        }
    }

    header: ToolBar {
        id: toolBar

        Label {
            id: pageTitle
            text: "Chat"
            font.pixelSize: 20
            anchors.centerIn: parent
        }

        Switch {
            id: serverSwitch
            text: qsTr("Server")
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.rightMargin: 0
            onClicked: {
                chatModel.switchMode(serverSwitch.checked)
            }
        }
    }
    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: messages
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: pane.leftPadding + messageField.leftPadding
            Layout.rightMargin: messages.leftMargin + 12
            displayMarginBeginning: 40
            displayMarginEnd: 40
            spacing: 12
            model: chatModel.messages
            onCountChanged: {
                if (count > 0) {
                    currentIndex = count - 1;
                }
            }
            delegate: Column {

                id: chat
                anchors.right: sentByMe ? parent.right : undefined
                spacing: 6
                TextField {
                    visible: !model.hasMessage && !model.hasFile
                    font.pixelSize: 10
                    placeholderText: model.sender + ( model.connected? " connected." : " disconnected.")
                }
                Row {
                    id: messageRow
                    spacing: 6
                    anchors.right: sentByMe ? parent.right : undefined
                    visible: model.message !== "" || model.hasFile
                    Label {
                        id: nickName
                        visible: !sentByMe
                        text: model.sender
                        font.bold: true
                    }
                    Rectangle {
                        width: Math.min( Math.max(messageText.implicitWidth, messageFileLable.implicitWidth) + 24,
                                        messages.width - messageRow.spacing)
                        height: (hasMessage? messageText.implicitHeight : 0) +
                                (hasFile? messageFileLable.implicitHeight : 0) + 24
                        color: sentByMe ? "lightgrey" : "steelblue"


                        Label {
                            id: messageText
                            text: model.message
                            color: sentByMe ? "black" : "white"
                            anchors.fill: parent
                            anchors.margins: 12
                            wrapMode: Label.Wrap
                        }
                        Label {
                            id: messageFileLable
                            visible: model.hasFile
                            text: "File: " + model.fileName
                            color: sentByMe ? "black" : "white"
                            font.bold: true
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 12
                            anchors.left: parent.left
                            anchors.leftMargin: 12
                            wrapMode: Label.Wrap

                            FileDialog{
                                id: saveFileDialog
                                fileMode: FileDialog.SaveFile
                                title: "Save file"
                                onAccepted: {
                                    chatModel.saveFile(file, model.file);
                                }
                            }
                        }
                    }
                    Button {
                        id: saveFileButton
                        text: "Save File"
                        visible: model.hasFile && !sentByMe
                        onClicked: {
                            saveFileDialog.open()
                        }
                    }

                }

            }
            ScrollBar.vertical: ScrollBar {

            }
        }
        Pane {
            id: pane
            Layout.fillWidth: true

            Column {
                id: column
                anchors.fill: parent
                spacing: 10

                RowLayout {
                    width: parent.width
                    spacing: 20

                    TextField {
                        id: messageField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Write message")
                        wrapMode: TextArea.Wrap
                        onEditingFinished: {
                            if(messageField.length == 0 && fileLable.text == "No file")
                                return
                            chatModel.sendMessage(messageField.text);
                            messageField.text = "";
                            fileLable.text = "No file";
                        }
                    }

                    Button {
                        id: sendButton
                        text: "Send"
                        enabled: messageField.length > 0 || fileLable.text != "No file"
                        onClicked: {
                            chatModel.sendMessage(messageField.text);
                            messageField.text = "";
                            fileLable.text = "No file";
                        }
                    }
                }

                RowLayout {
                    width: parent.width
                    spacing: 20

                    Label {
                        id: fileLable
                        text: "No File"
                        padding: 10
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        Layout.fillWidth: true
                    }

                    Button {
                        id: attachFileButton
                        text: "Attach file"
                        onClicked: {
                            openFileDialog.open();
                        }
                    }
                }
            }

        }


    }
}


/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:16;anchors_height:400;anchors_width:200}
D{i:20;anchors_height:100;anchors_width:100}D{i:3;anchors_y:0}
}
##^##*/
