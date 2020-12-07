#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H
#include <QObject>

#include <QMap>
#include <QMutex>
#include <QSharedPointer>
#include <QTcpSocket>

class SocketManager
    : public QObject
{
private:
    static SocketManager* singleIsntace;
    static QMutex mutex;

    QMap < QPair <const QString, const quint16>, QSharedPointer <QTcpSocket>> socketMap;

    SocketManager();
public:
    static SocketManager* getInstance();

    QTcpSocket* getSocket(const QString& addres, const quint16& port);
};

#endif // SOCKETMANAGER_H
