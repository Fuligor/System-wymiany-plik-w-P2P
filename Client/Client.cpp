#include "Client.h"

#include <QApplication>
#include <QDateTime>

#include <iostream>

void toString(quint64 value, char* string)
{
    for(int i = 0; i < 8; i++)
    {
        string[i] = value & (0xFF);
        value = value >> 8;
    }
}

Client::Client()
    :QObject(nullptr), myId(createId())
{
}

const std::string Client::createId()
{
    char id[20] = "-TA-";

    toString(QApplication::instance()->applicationPid(), id + 4);
    toString(QDateTime::currentDateTime().toMSecsSinceEpoch(), id + 12);

    std::cout << QApplication::instance()->applicationPid() << std::endl;
    std::cout << QDateTime::currentDateTime().toMSecsSinceEpoch() << std::endl;

    return std::string(id, 20);
}

Client *Client::getInstance()
{
    mutex.lock();

    if(sigleInstance == nullptr)
    {
        sigleInstance = new Client();
    }

    mutex.unlock();

    return sigleInstance;
}

const std::string& Client::getId() const
{
    return myId;
}

Client* Client::sigleInstance = nullptr;
QMutex Client::mutex;
