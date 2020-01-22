#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>


struct Message{
    QString senderName;
    QString message;
    QString fileName;
    QByteArray file;
    bool sentByMe;
    bool connected;
};

struct ChangingUserList
{
    QString userName;
    bool connected;//or disconnected
};

enum class DataType : int{
    message,
    users
};

#endif // MESSAGE_H
