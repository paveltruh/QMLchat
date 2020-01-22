#include "messageslist.h"

MessagesList::MessagesList(QObject *parent)
    : QAbstractListModel(parent)
{

}

int MessagesList::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }

    return _messages.size();
}

QVariant MessagesList::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch (role) {
    case SenderRole:
        return QVariant(_messages[index.row()].senderName);
    case ReceiverRole:
        return QVariant("Receiver");
    case HasFileRole:
        return QVariant(!_messages[index.row()].fileName.isEmpty());
    case FileRole:
        return QVariant(_messages[index.row()].file);
    case HasMessageRole:
        return QVariant(!_messages[index.row()].message.isEmpty());
    case MessageRole:
        return QVariant(_messages[index.row()].message);
    case SentByMe:
        return QVariant(_messages[index.row()].sentByMe);
    case Connected:
        return QVariant(_messages[index.row()].connected);
    case FileName:
        return QVariant(_messages[index.row()].fileName);
    }
    return QVariant();
}

QHash<int, QByteArray> MessagesList::roleNames() const
{
    QHash<int, QByteArray> names;
    names[SenderRole] = "sender";
    names[ReceiverRole] = "receiver";
    names[HasFileRole] = "hasFile";
    names[FileRole] = "file";
    names[HasMessageRole] = "hasMessage";
    names[MessageRole] = "message";
    names[SentByMe] = "sentByMe";
    names[Connected] = "connected";
    names[FileName] = "fileName";
    return names;
}

void MessagesList::append(const Message& message)
{
    beginInsertRows(QModelIndex(), _messages.size(), _messages.size());
    _messages.push_back(message);
    endInsertRows();
}
