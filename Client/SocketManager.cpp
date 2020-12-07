#include "SocketManager.h"

SocketManager::SocketManager()
    :QObject (nullptr)
{}

SocketManager *SocketManager::getInstance()
{
    mutex.lock();

    if(singleIsntace == nullptr)
    {
        singleIsntace = new SocketManager();
    }

    mutex.unlock();

    return singleIsntace;
}

QTcpSocket *SocketManager::getSocket(const QString& addres, const quint16& port)
{
    const QPair <QString, quint16> key(addres, port);

    if(!socketMap.contains(key))
    {
        socketMap[key] = QSharedPointer <QTcpSocket> (new QTcpSocket(this));
        socketMap[key]->connectToHost(addres, port);
    }

    return socketMap[key].get();
}

SocketManager* SocketManager::singleIsntace = nullptr;
QMutex SocketManager::mutex;
