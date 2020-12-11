#ifndef TRACKERCONNECTION_H
#define TRACKERCONNECTION_H

#include <QObject>
#include <QString>

#include "torrentReader.h"
#include "TrackerRequest.h"

class QTcpSocket;
class QTimer;

class TrackerConnection
        : public QObject
{
    Q_OBJECT

private:
    QTcpSocket* socket;
    QTimer* requestTimer;
    torrentReader torrent;
    TrackerRequest request;
    bool isActive = false;
public:
    TrackerConnection(const std::string& fileName);
    ~TrackerConnection();
protected:
    void initRequest();
    void connectToTracker();
    void sendRequest();

public slots:
    void start();
    void stop();
    void completed();
protected slots:
    void interval();

public slots:
    void read();
signals:
    quint32 readCompeted();
};

#endif // TRACKERCONNECTION_H
