#include "messagesmodel.h"

#include <QFile>


MessagesModel::MessagesModel(QObject *parent)
    : QObject(parent),
      _controller(new ChatController(this)),
      _messages(new MessagesList(this))
{
    connect(_controller, SIGNAL(messageReceived(Message&)),
            SLOT(receiveMessage(Message&)));
    connect(_controller, SIGNAL(greetingMessageReceived(ChangingUserList&)),
            SLOT(receiveGreeetingMessage(ChangingUserList&)));
    connect(_controller, SIGNAL(connectedToServer(QString)),
            _controller,SLOT(sendGreetingMessage()));
}

MessagesList *MessagesModel::messagesModel() const
{
    return _messages;
}

void MessagesModel::joinToChat(const QString& userName)
{
    _controller->setUserName(userName);
    qDebug() << "3";
    _controller->sendGreetingMessage();
}

void MessagesModel::sendMessage(const QString& message)
{
    if(message.isEmpty() && _controller->getFileName().isEmpty())
        return;
    Message mes;
    mes.sentByMe = true;
    mes.message = message;
    mes.senderName = _controller->getUserName();
    mes.fileName = _controller->getFileName();
    _controller->sendMessege(message);
    _messages->append(mes);
}

void MessagesModel::switchMode(bool isServer)
{
    if(isServer)
        _controller->switchToServerMode();
    else
        _controller->switchToClientMode();
}

QString MessagesModel::openFile(QString path)
{
    qDebug() <<"Load file: " + path;
    path = path.mid(8,path.length()-8);
    QString fileName = _controller->attachFile(path);
    if(!fileName.isEmpty()){
        emit fileOpened(fileName);
        return fileName;
    }
    return "No file";
}

void MessagesModel::saveFile(QString path,const QByteArray& data)
{
    qDebug() <<"Save file: " + path;
    path = path.mid(8,path.length()-8);
    if(path.isEmpty())
        return;
    QFile file(path);
    if(!file.open(QFile::WriteOnly))
        return;
    file.write(data);
}

void MessagesModel::receiveMessage(Message& message)
{
    message.sentByMe = false;
    _messages->append(message);
    emit messageReceived(message);
}

void MessagesModel::receiveGreeetingMessage(ChangingUserList& changing)
{
    Message message;
    message.connected = changing.connected;
    message.senderName = changing.userName;
    message.sentByMe = false;
    _messages->append(message);
    emit messageReceived(message);
}
