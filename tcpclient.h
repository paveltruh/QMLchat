#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDataStream>
#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include "message.h"

class TCPClient : public QObject
{
    Q_OBJECT
    QTcpSocket* _client = nullptr;
    QDataStream _in;

public:
    explicit TCPClient(QObject *parent = nullptr);
    void disconnectFromServer();
    bool isConnected();
    void sendMessage(Message&);
    void sendGreetingMessage(ChangingUserList&);

signals:
    void connected(QString);
    void disconnected();
    void messageReceived(Message&);
    void greetingMessageReceived(ChangingUserList&);

public slots:
    void connectToServer(QHostAddress host, quint16 port);

private slots:
    void myRead();
    void debugSocketState(QAbstractSocket::SocketState);
    void connectedSlot();
    void disconnectedSlot();
};

#endif // TCPCLIENT_H
