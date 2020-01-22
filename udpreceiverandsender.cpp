#include "udpreceiverandsender.h"

#include <QCborStreamReader>
#include <QHostInfo>
#include <QNetworkAddressEntry>

UDPReceiverAndSender::UDPReceiverAndSender(QObject *parent) : QObject(parent)
{
    _serverPort = 0;
    setLocalIp();
    _udpSocket = new QUdpSocket(this);
    updateAddresses();
    _udpSocket->bind(QHostAddress::Any, _broadcastingPort, QUdpSocket::ShareAddress
                         | QUdpSocket::ReuseAddressHint);
    _isListening = true;
    connect(_udpSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));
    //udpSocket->setInterval(BroadcastInterval);
    //connect(startButton, SIGNAL(clicked(bool)), SLOT(startBroadcasting()));
    //connect(quitButton, SIGNAL(clicked(bool)), SLOT(close()));
    connect(&_timer, SIGNAL(timeout()), SLOT(broadcastDatagram()));
//    connect(&broadcastTimer, SIGNAL(timeout()),
    //            this, SLOT(sendBroadcastDatagram()));
}

bool UDPReceiverAndSender::startBroadcasting()
{
    if(_serverPort==0)
        return false;
    _timer.start(250);
    return true;
}

void UDPReceiverAndSender::stopBroadcasting()
{
    _timer.stop();
}

void UDPReceiverAndSender::continueListening()
{
//    if(udpSocket->pauseMode() != QAbstractSocket::PauseNever){
//        udpSocket->resume();
//        udpSocket->bind(QHostAddress::Any, _broadcastingPort, QUdpSocket::ShareAddress
//                             | QUdpSocket::ReuseAddressHint);
//    }
    _isListening = true;
    _udpSocket->abort();
    _udpSocket->bind(QHostAddress::Any, _broadcastingPort, QUdpSocket::ShareAddress
                         | QUdpSocket::ReuseAddressHint);
    //updateAddresses();
//    //delete udpSocket;
//    udpSocket = new QUdpSocket();
//    udpSocket->bind(QHostAddress::Any, _broadcastingPort, QUdpSocket::ShareAddress
//                         | QUdpSocket::ReuseAddressHint);
//    connect(udpSocket, SIGNAL(readyRead()),
//            this, SLOT(processPendingDatagrams()));
}

void UDPReceiverAndSender::pauseListening()
{
    //udpSocket->setPauseMode(QAbstractSocket::PauseOnSslErrors);
    _isListening = false;
}

void UDPReceiverAndSender::setServerPort(quint16 serverPort)
{
    _serverPort = serverPort;
}



void UDPReceiverAndSender::setLocalIp()
{

    QString localhostname =  QHostInfo::localHostName();
    QList<QHostAddress> hostList = QHostInfo::fromName(localhostname).addresses();
    foreach (const QHostAddress& address, hostList) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address.isLoopback() == false)
            _localIP = address;
    }
    //qDebug() << QString::number(localIP.toIPv4Address());
}

void UDPReceiverAndSender::processPendingDatagrams()
{
    while (_isListening && _udpSocket!=nullptr
           && _udpSocket->hasPendingDatagrams()) {
        QHostAddress host;
        quint16 senderPort;
        QByteArray datagram;
        datagram.resize(_udpSocket->pendingDatagramSize());
        if (_udpSocket->readDatagram(datagram.data(), datagram.size(),
                                    &host, &senderPort) == -1)
            continue;
        //qDebug() << "Received greeting datagram ";
        //if (isLocalHostAddress(host))
            //continue;
//        if(senderIp.toIPv4Address() == localIP.toIPv4Address())
//            continue;
        int senderServerPort;
        {
            QCborStreamReader reader(datagram);
            if (reader.lastError() != QCborError::NoError || !reader.isArray())
                continue;
            if (!reader.isLengthKnown() || reader.length() != 2)
                continue;

            reader.enterContainer();
            if(!reader.isInteger())
                continue;
            quint64 temp = reader.toUnsignedInteger();
            if(!reader.hasNext())
                continue;

            reader.next();
            if(!reader.isInteger())
                continue;
            senderServerPort = reader.toInteger();
//            if(serverPort == senderServerPort)
//                continue;
            //statusLabel->setText(tr("Received datagram: \"%1\"")
                                 //.arg(senderServerPort));
            if(isLocalHostAddress(host) && senderServerPort == _serverPort)
                continue;
            qDebug() << "Received greeting datagram from " + QString::number(host.toIPv4Address());
            emit greetingDatagramReceived(host, senderServerPort);
            //tryingToConnectToTCP = true;
            _isListening = false;
        }

        //statusLabel->setText(senderIp.toString() + ":" + QString::number( senderServerPort));
    }
}


void UDPReceiverAndSender::broadcastDatagram()
{
    QByteArray datagram;

    {
        QCborStreamWriter writer(&datagram);
        writer.startArray(2);
        writer.append(_localIP.toIPv4Address());
        writer.append(_serverPort);
        writer.endArray();
    }

    bool validBroadcastAddresses = true;
    for (const QHostAddress &address : qAsConst(_broadcastAddresses)) {
        if (_udpSocket->writeDatagram(datagram, address,
                                          _broadcastingPort) == -1)
            validBroadcastAddresses = false;
    }
    //qDebug() << "Sent greeting datagram ";

    if (!validBroadcastAddresses)
        updateAddresses();
    //datagramSent(QString::number(localIP.toIPv4Address()) + );
}

bool UDPReceiverAndSender::isLocalHostAddress(const QHostAddress &address)  const
{
    for (const QHostAddress &localAddress : _ipAddresses) {
        if (address.isEqual(localAddress))
            return true;
    }
    return false;
}

void UDPReceiverAndSender::updateAddresses()
{
    _broadcastAddresses.clear();
    _ipAddresses.clear();
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &interface : interfaces) {
        const QList<QNetworkAddressEntry> entries = interface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            QHostAddress broadcastAddress = entry.broadcast();
            if (broadcastAddress != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost) {
                _broadcastAddresses << broadcastAddress;
                _ipAddresses << entry.ip();
            }
        }
    }
}
