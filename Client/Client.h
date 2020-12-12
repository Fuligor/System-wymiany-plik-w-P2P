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
    TrackerConnection *connection;

    explicit Client();
public:
    static Client *getInstance();
};

#endif // CLIENT_H
