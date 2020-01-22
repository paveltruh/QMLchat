#include "tcpserver.h"

#include <QDataStream>


TCPServer::TCPServer(QObject *parent) : QObject(parent)
{
    _server = new QTcpServer(this);
    connect(_server, SIGNAL(newConnection()), this, SLOT(newUser()));
    if (!_server->listen(QHostAddress::Any) /*&& server_status==0*/) {
        qDebug() <<  QObject::tr("Unable to start the server: %1.").arg(_server->errorString());
    } else {
        //server_status=1;
        //qDebug() << server->isListening() << "TCPSocket listen on port";
        qDebug() << "TCP server started!";
    }
}

quint16 TCPServer::getPort()
{
    return _server->serverPort();
}

void TCPServer::stopServer()
{
    foreach(int i,_clients.keys()){
        _clients[i].first->close();
        _clients.remove(i);
    }
    _server->close();
    qDebug() << "TCP server stoped";
}

void TCPServer::sendMessage(Message& message, int idusersocs)
{
    QByteArray byte;
    QDataStream data(&byte,QIODevice::WriteOnly);

    data << static_cast<int> (DataType::message);
    data << message.senderName;
    data << message.message;
    data << message.fileName;
    if(!message.fileName.isEmpty())
        data << message.file;

    foreach(int i,_clients.keys())
        if(i!=idusersocs && !_clients[i].second.isEmpty())
            _clients[i].first->write(byte);
    qDebug() << "Message send from server";
}

void TCPServer::sendGreetingMessage(ChangingUserList changing, int idusersocs)
{
    QByteArray byte;
    QDataStream data(&byte,QIODevice::WriteOnly);

    data << static_cast<int> (DataType::users);
    data << changing.userName;
    data << changing.connected;
    foreach(int i,_clients.keys())
        if(i!=idusersocs && !_clients[i].second.isEmpty())
            _clients[i].first->write(byte);
    qDebug() << "Greeting message send from server";
}

bool TCPServer::isConnected()
{
    return !_clients.isEmpty();
}

void TCPServer::readClientMessage()
{
    QTcpSocket* client = (QTcpSocket*)sender();
    int idusersocs=client->socketDescriptor();
    _in.setDevice(client);
    _in.startTransaction();
    int type;
    _in >> type;
    if(type == static_cast<int>(DataType::message)){
        Message message;
        _in >> message.senderName;
        _in >> message.message;
        _in >> message.fileName;
        if(!message.fileName.isEmpty())
            _in >> message.file;
        qDebug() << "Message receiving";
        if (!_in.commitTransaction())
            return;     // wait for more data
        emit messageReceived(message);
        qDebug() << "Message received";

        sendMessage(message, idusersocs);
    }
    else if(type == static_cast<int>(DataType::users)){
        ChangingUserList changing;
        _in >> changing.userName;
        _in >> changing.connected;
        if (!_in.commitTransaction())
            return;     // wait for more data
        emit greetingMessageReceived(changing);
        qDebug() << "Greeting message received";
        _clients[idusersocs].second = changing.userName;
        sendGreetingMessage(changing, idusersocs);
    }
}


void TCPServer::newUser()
{
    qDebug() << "New connection";
    QTcpSocket* clientSocket = _server->nextPendingConnection();
    int idusersocs=clientSocket->socketDescriptor();
    _clients[idusersocs].first = clientSocket;
    connect(_clients[idusersocs].first,SIGNAL(readyRead()),
            SLOT(readClientMessage()));
    connect(_clients[idusersocs].first,&QTcpSocket::disconnected,
            [=](){
        ChangingUserList changing {_clients[idusersocs].second,false};
        sendGreetingMessage(changing,idusersocs);
        emit greetingMessageReceived(changing);
        _clients.remove(idusersocs);
    });
}
