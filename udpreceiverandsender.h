#ifndef UDPRECEIVERANDSENDER_H
#define UDPRECEIVERANDSENDER_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

class UDPReceiverAndSender : public QObject
{
    Q_OBJECT
    bool _isListening;
    quint16 _serverPort = 0;
    const quint16 _broadcastingPort = 45454;
    QUdpSocket *_udpSocket = nullptr;
    QList<QHostAddress> _broadcastAddresses;
    QList<QHostAddress> _ipAddresses;
    QHostAddress _localIP;
    QTimer _timer;

public:
    explicit UDPReceiverAndSender(QObject *parent = nullptr);
    bool startBroadcasting();
    void stopBroadcasting();
    void continueListening();
    void pauseListening();
    void setServerPort(quint16 serverPort);

signals:
    void datagramSent(QString datagram);
    void greetingDatagramReceived(QHostAddress host, quint16 port);

private slots:
    void processPendingDatagrams();
    void broadcastDatagram();

private:
    bool isLocalHostAddress(const QHostAddress &address) const;
    void updateAddresses();
    void setLocalIp();
};

#endif // UDPRECEIVERANDSENDER_H
