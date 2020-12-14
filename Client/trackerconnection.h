#ifndef TRACKERCONNECTION_H
#define TRACKERCONNECTION_H
#include <QObject>

#include <list>

#include <QWaitCondition>
#include <QString>
#include <QMutex>

#include "Bencode.h"
#include "Peer.h"
#include "torrentReader.h"
#include "TrackerRequest.h"

class QTcpSocket;
class QTimer;

class TrackerConnection
        : public QObject
{
    Q_OBJECT

private:
    enum class State
    {
        INIT,
        ACTIVE,
        AWAITING,
        CLOSED
    } currentState;

    QMutex* mutex;
    QWaitCondition* inActiveState;
    QTcpSocket* socket;
    QTimer* requestTimer;
    torrentReader torrent;
    TrackerRequest request;
    bencode::Utf8Decoder utf8decoder;
    bencode::Decoder decoder;
    std::wstring buf;
    bool isUpdateSheduled;
    bool isTimerTimeouted;
public:
    TrackerConnection(const std::string& fileName);
    ~TrackerConnection();
protected:
    void initRequest();
    void connectToTracker();
    void sendRequest();

public slots:
    void startRequest();
    void stopRequest();
    void completeRequest();
    void updatePeerList();

protected slots:
    void interval();
    void regularRequest();

public slots:
    void read();

protected:
    void decodeResponse();

signals:
    void trackerDisconnected();
    void peerListUpdated(bencode::List peerList);
};

#endif // TRACKERCONNECTION_H
