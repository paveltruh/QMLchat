#include "chatcontroller.h"

#include <QFile>
#include <QFileInfo>

QString ChatController::getUserName() const
{
    return _userName;
}

QString ChatController::getFileName() const
{
    return _fileName;
}

void ChatController::sendGreetingMessage()
{
    if(!isConnected())
        return;
    ChangingUserList changing;
    changing.userName = _userName;
    changing.connected = true;
    if(_mode == TCPMode::server && _serverSocket)
        _serverSocket->sendGreetingMessage(changing);
    else if(_mode == TCPMode::client && _clientSocket)
        _clientSocket->sendGreetingMessage(changing);
}

bool ChatController::isConnected()
{
    if(_mode == TCPMode::server && _serverSocket)
        return _serverSocket->isConnected();
    else if(_mode == TCPMode::client && _clientSocket)
        return _clientSocket->isConnected();
    else
        return false;
}

ChatController::ChatController(QObject *parent) : QObject(parent)
{
    _mode = TCPMode::client;
    _receiverAndSender = new UDPReceiverAndSender(this);
    connect(_receiverAndSender, SIGNAL(greetingDatagramReceived(QHostAddress,quint16)),
            SLOT(tryToConnectToServer(QHostAddress,quint16)));

    _clientSocket = new TCPClient(this);
    _serverSocket = nullptr;
    connect(_clientSocket,SIGNAL(connected(QString)),SIGNAL(connectedToServer(QString)));
    connect(_clientSocket,SIGNAL(disconnected()),SLOT(continueListening()));
    connect(_clientSocket,SIGNAL(disconnected()),SIGNAL(disconnectedFromServer()));
    connect(_clientSocket,SIGNAL(messageReceived(Message&)),SIGNAL(messageReceived(Message&)));
    connect(_clientSocket,SIGNAL(greetingMessageReceived(ChangingUserList&)),
            SIGNAL(greetingMessageReceived(ChangingUserList&)));

    //connect(&connectingTimer,SIGNAL(timeout()),SLOT(attemptToConnectFailed()));
    //QTimer::singleShot(3000, this, SLOT(autoSwitchToerverMode()) );
}

QString ChatController::attachFile(QString path)
{
    if(path.isEmpty())
        return QString();
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
        return QString();
    this->_file = file.readAll();
    QFileInfo fileInfo(file.fileName());
    _fileName = fileInfo.fileName();
    return _fileName;
}

void ChatController::setUserName(QString userName)
{
    this->_userName = userName;
}

void ChatController::sendMessege(const QString &message)
{
    Message mes;
    mes.message = message;
    mes.senderName = _userName;
    mes.fileName = _fileName;
    mes.file = _file;
    _fileName.clear();
    _file.clear();
    if(_mode == TCPMode::server)
        sendMessegeFromServer(mes);
    else if(_mode == TCPMode::client)
        sendMessegeFromClient(mes);
}

void ChatController::switchToServerMode()
{
    if(_mode == TCPMode::server)
        return;
    if(!_clientSocket){
        qDebug() << "TCP client error";
        return;
    }
    if(!_receiverAndSender){
        qDebug() << "UDP receiver and sender error";
        return;
    }
    _clientSocket->disconnectFromServer();
    if(!_serverSocket){
        _serverSocket = new TCPServer(this);
        connect(_serverSocket,SIGNAL(messageReceived(Message&)),SIGNAL(messageReceived(Message&)));
        connect(_serverSocket,SIGNAL(greetingMessageReceived(ChangingUserList&)),
                SIGNAL(greetingMessageReceived(ChangingUserList&)));
    }
    _receiverAndSender->pauseListening();
    _receiverAndSender->setServerPort(_serverSocket->getPort());
    _receiverAndSender->startBroadcasting();
    _mode = TCPMode::server;
}

void ChatController::switchToClientMode()
{
    if(_mode == TCPMode::client)
        return;
    if(!_serverSocket){
        qDebug() << "TCP server error";
        return;
    }
    if(!_receiverAndSender){
        qDebug() << "UDP receiver and sender error";
        return;
    }
    _receiverAndSender->stopBroadcasting();
    _receiverAndSender->continueListening();
    _serverSocket->stopServer();
    _serverSocket->deleteLater();
    _serverSocket = nullptr;
    if(!_clientSocket){
        _clientSocket = new TCPClient(this);
        connect(_clientSocket,SIGNAL(connected(QString)),SIGNAL(connectedToServer(QString)));
        connect(_clientSocket,SIGNAL(disconnected()),SLOT(continueListening()));
        connect(_clientSocket,SIGNAL(disconnected()),SIGNAL(disconnectedFromServer()));
        connect(_clientSocket,SIGNAL(messageReceived(Message&)),SIGNAL(messageReceived(Message&)));
        connect(_clientSocket,SIGNAL(greetingMessageReceived(ChangingUserList&)),
                SIGNAL(greetingMessageReceived(ChangingUserList&)));
    }
    _mode = TCPMode::client;
}

void ChatController::autoSwitchToerverMode()
{
    if(_mode == TCPMode::client &&
         _clientSocket && !_clientSocket->isConnected()){
        switchToServerMode();
        emit switchedToServerMode();
    }
}

void ChatController::continueListening()
{
    if(_mode == TCPMode::client)
        _receiverAndSender->continueListening();
//    receiverAndSender = new UDPReceiverAndSender(this);
//    connect(receiverAndSender, SIGNAL(greetingDatagramReceived(QHostAddress,quint16)),
    //            clientSocket, SLOT(connectToServer(QHostAddress,quint16)));
}

void ChatController::attemptToConnectFailed()
{
    _connectingTimer.stop();
    if(!_clientSocket){
        qDebug() << "TCP client error";
        return;
    }
    if(_clientSocket->isConnected()){
        qDebug() << "Already connected";
        return;
    }
    qDebug() << "Attempt to connect failed";
    if(_mode != TCPMode::client)
        return;
    _clientSocket->disconnectFromServer();
    _receiverAndSender->continueListening();
}

void ChatController::tryToConnectToServer(QHostAddress host, quint16 port)
{
    _clientSocket->connectToServer(host,port);
    _connectingTimer.start(200);
}



void ChatController::sendMessegeFromServer(Message &message)
{
    if(_serverSocket)
        _serverSocket->sendMessage(message);
}

void ChatController::sendMessegeFromClient(Message& message)
{
    if(_clientSocket)
        _clientSocket->sendMessage(message);
}
