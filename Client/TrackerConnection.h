#ifndef TRACKERCONNECTION_H
#define TRACKERCONNECTION_H
#include <QObject>

#include <list>

#include <QWaitCondition>
#include <QString>
#include <QMutex>

#include "Bencode.h"
#include "ConnectionStatus.h"
#include "Peer.h"
#include "torrentReader.h"
#include "TrackerRequest.h"

class QTcpSocket;
class QTimer;

class TorrentDownloader;

class TrackerConnection
        : public QObject
{
    Q_OBJECT

private:
    ConnectionStatus currentState;

    QMutex* mutex;
    QWaitCondition* inActiveState;
    QTcpSocket* socket;
    QTimer* requestTimer;
    QTimer* connectTimer;
    TrackerRequest request;
    bencode::Utf8Decoder utf8decoder;
    bencode::Decoder decoder;
    std::wstring buf;
    bool isUpdateSheduled;
    bool isTimerTimeouted;

    QString adress;
    quint16 port;
    quint16 listenerPort;
    bool firstConnectionTry = true;
public:
    TrackerConnection(const std::shared_ptr <bencode::Dict>& torrentDict, quint16 listenerPort, TorrentDownloader* parent);
    ~TrackerConnection();

    void setLeft(size_t left);
    void setDownloaded(size_t downloaded);
    void setUploaded(size_t uploaded);
protected:
    void initRequest();
    void initConnection(const std::shared_ptr <bencode::Dict>& torrentDict);
    void sendRequest();
    void setState(ConnectionStatus state);

public slots:
    void startRequest();
    void stopRequest();
    void completeRequest();
    void updatePeerList();
    void connectToTracker();

protected slots:
    void interval();
    void regularRequest();
    void onConnection();
    void retryConnect();
    void reconnect();

public slots:
    void read();

protected:
    void decodeResponse();

signals:
    void trackerDisconnected();
    void statusChanged(const ConnectionStatus& status);
    void peerListUpdated(bencode::List peerList);
};

#endif // TRACKERCONNECTION_H
