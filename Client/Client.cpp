#include "Client.h"

#include <QApplication>
#include <QDateTime>

Client::Client()
    :QObject(nullptr), myId(createId())
{
}

const std::string Client::createId()
{
    char id[20] = "-TA-";

    *((quint64*) id + 4) = QApplication::instance()->applicationPid();
    *((quint64*) id + 12) = QDateTime::currentDateTime().toMSecsSinceEpoch();

    std::cout << QApplication::instance()->applicationPid() << std::endl;
    std::cout << QDateTime::currentDateTime().toMSecsSinceEpoch() << std::endl;

    std::cout << *((quint64*) id + 4) << std::endl;
    std::cout << *((quint64*) id + 12) << std::endl;

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
