import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3

Page {
    width: 600
    height: 400
    anchors.fill: parent

    title: qsTr("Login")


    ColumnLayout {
        id: columnLayout
        anchors.centerIn: parent
        Label {
            id: label
            text: "Welcome to chat"
        }

        RowLayout {
            id: rowLayout
            width: 100
            height: 100

            TextField {
                id: nameField
                placeholderText: "Text Field"
                onEditingFinished: {
                    if(nameField.text!="")
                        window.join(nameField.text)
                }
            }

            Button {
                id: goButton
                text: qsTr("Join")
                enabled: nameField.length > 0
                onClicked: {
                    window.join(nameField.text)
                }
            }
        }
    }
}
