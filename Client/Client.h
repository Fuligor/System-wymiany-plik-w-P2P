#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QMutex>

#include "TrackerConnection.h"

class Client : public QObject
{
    Q_OBJECT
private:
    static Client* sigleInstance;
    static QMutex mutex;
    const std::string myId;

    Client();
protected:
    const std::string createId();
public:
    static Client *getInstance();
    const std::string& getId() const;
};

#endif // CLIENT_H
