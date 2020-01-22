#ifndef MESSAGESLIST_H
#define MESSAGESLIST_H

#include <QAbstractListModel>
#include <QObject>
#include "message.h"

class MessagesList : public QAbstractListModel
{
    Q_OBJECT
    QVector<Message> _messages;

public:
    MessagesList(QObject *parent = nullptr);

    enum {
        SenderRole = Qt::UserRole,
        ReceiverRole,
        HasMessageRole,
        MessageRole,
        HasFileRole,
        FileRole,
        SentByMe,
        Connected,
        FileName
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void append(const Message&);
};

#endif // MESSAGESLIST_H
