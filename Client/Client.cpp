#include "client.h"

Client::Client()
    :QObject(nullptr)
{
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

Client* Client::sigleInstance = nullptr;
QMutex Client::mutex;
