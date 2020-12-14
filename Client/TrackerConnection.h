#ifndef TRACKERCONNECTION_H
#define TRACKERCONNECTION_H

#include <QObject>
#include <QString>

#include "torrentReader.h"
#include "TrackerRequest.h"
#include "Bencode.h"

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
    bencode::Utf8Decoder utf8decoder;
    bencode::Decoder decoder;
    std::wstring buf;
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
protected:
    void decodeResponse();
signals:
    quint32 readCompeted();
};

#endif // TRACKERCONNECTION_H
