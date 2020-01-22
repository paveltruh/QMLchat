#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDataStream>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "message.h"

class TCPServer : public QObject
{
    Q_OBJECT
    QTcpServer* _server = nullptr;
    QMap<int,QPair<QTcpSocket*,QString>> _clients;
    QTcpSocket* _socket = nullptr;
    QDataStream _in;

public:
    explicit TCPServer(QObject *parent = nullptr);
    quint16 getPort();
    void stopServer();
    void sendMessage(Message&, int idusersocs = -1);
    void sendGreetingMessage(ChangingUserList, int idusersocs = -1);
    bool isConnected();

signals:
    void messageReceived(Message&);
    void greetingMessageReceived(ChangingUserList&);

private slots:
    void readClientMessage();
    void newUser();
};

#endif // TCPSERVER_H
