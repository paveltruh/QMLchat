#ifndef CHATCONTROLER_H
#define CHATCONTROLER_H

#include <QObject>
#include "udpreceiverandsender.h"
#include "tcpclient.h"
#include "tcpserver.h"
#include "message.h"

class ChatController : public QObject
{
    Q_OBJECT
    UDPReceiverAndSender* _receiverAndSender;
    TCPClient* _clientSocket;
    TCPServer* _serverSocket;
    enum class TCPMode{
        client,
        server
    };
    TCPMode _mode;
    QString _fileName;
    QByteArray _file;
    QString _userName;
    QTimer _connectingTimer;

public:
    explicit ChatController(QObject *parent = nullptr);
    QString attachFile(QString);
    void setUserName(QString);
    QString getUserName() const;
    QString getFileName() const;
    bool isConnected();

signals:
    void connectedToServer(QString);
    void disconnectedFromServer();
    void messageReceived(Message&);
    void switchedToServerMode();
    void greetingMessageReceived(ChangingUserList&);

public slots:
    void sendGreetingMessage();
    void sendMessege(const QString &);
    void switchToServerMode();
    void switchToClientMode();

private slots:
    void autoSwitchToerverMode();
    void continueListening();
    void attemptToConnectFailed();
    void tryToConnectToServer(QHostAddress,quint16);

private:
    void sendMessegeFromServer(Message&);
    void sendMessegeFromClient(Message&);
};

#endif // CHATCONTROLER_H
