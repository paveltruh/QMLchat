#include "tcpclient.h"

#include <QHostAddress>

TCPClient::TCPClient(QObject *parent) : QObject(parent)
{
    _client = new QTcpSocket(this);
    connect(_client,SIGNAL(disconnected()),SLOT(disconnectedSlot()));
    connect(_client, SIGNAL(connected()), SLOT(connectedSlot()));
    connect(_client, SIGNAL(readyRead()),SLOT(myRead()));
    connect(_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            SLOT(debugSocketState(QAbstractSocket::SocketState)));
}

void TCPClient::disconnectFromServer()
{
    _client->disconnectFromHost();
    //disconnect(client, SIGNAL(connected()), SLOT(connectedSlot()));
}

bool TCPClient::isConnected()
{
    if(!_client)
        return false;
    return _client->peerAddress() != QHostAddress::Null;
}

void TCPClient::sendMessage(Message& message)
{
    QByteArray byte;
    QDataStream data(&byte,QIODevice::WriteOnly);

    data << static_cast<int> (DataType::message);
    data << message.senderName;
    data << message.message;
    data << message.fileName;
    if(!message.fileName.isEmpty())
        data << message.file;

    _client->write(byte);


    qDebug() << "Message send";
}

void TCPClient::sendGreetingMessage(ChangingUserList& changing)
{
    QByteArray byte;
    QDataStream data(&byte,QIODevice::WriteOnly);

    data << static_cast<int> (DataType::users);
    data << changing.userName;
    data << changing.connected;

    _client->write(byte);


    qDebug() << "Greeting message send";
}

void TCPClient::connectToServer(QHostAddress host, quint16 port)
{
    _client->connectToHost(host, port);
    qDebug() << "Try to connect to " + QString::number(host.toIPv4Address());
}

void TCPClient::myRead()
{
    _in.setDevice(_client);
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
    }
    else if(type == static_cast<int>(DataType::users)){
        ChangingUserList changing;
        _in >> changing.userName;
        _in >> changing.connected;
        if (!_in.commitTransaction())
            return;     // wait for more data
        emit greetingMessageReceived(changing);
        qDebug() << "Greeting message received";
    }
}

void TCPClient::debugSocketState(QAbstractSocket::SocketState state)
{
    qDebug() << "client : " << state;
}

void TCPClient::connectedSlot()
{
    emit connected(_client->peerAddress().toString() + ":" +
                   QString::number(_client->peerPort()));
    qDebug() << "Connected";
}

void TCPClient::disconnectedSlot()
{
    //auto a = client->state();
    qDebug() << "Disconnected";
    emit disconnected();
    _client->disconnectFromHost();
    _client->deleteLater();
    _client = new QTcpSocket(this);
    connect(_client,SIGNAL(disconnected()),SLOT(disconnectedSlot()));
    connect(_client, SIGNAL(connected()), SLOT(connectedSlot()));
    connect(_client, SIGNAL(readyRead()),SLOT(myRead()));
    connect(_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            SLOT(debugSocketState(QAbstractSocket::SocketState)));
}
