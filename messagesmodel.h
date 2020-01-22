#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include <QAbstractListModel>
#include "chatcontroller.h"
#include "messageslist.h"


class MessagesModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MessagesList* messages READ messagesModel FINAL)

    ChatController* _controller;
    MessagesList* _messages;

public:
    explicit MessagesModel(QObject *parent = nullptr);
    MessagesList* messagesModel() const;

public slots:
    void joinToChat(const QString&);
    void sendMessage(const QString&);
    void switchMode(bool isServer);
    QString openFile(QString path);
    void saveFile(QString path, const QByteArray &file);

signals:
    void messageReceived(Message&);
    void isServerChanged();
    void fileOpened(QString);

private slots:
    void receiveMessage(Message&);
    void receiveGreeetingMessage(ChangingUserList&);
};

#endif // MESSAGESMODEL_H
