#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include <QObject>
#include <set>
#include <list>

#include <QString>

#include "Types.h"
#include "Bencode.h"
#include "ConnectionStatus.h"
#include "Peer.h"
#include "File.h"
#include "torrentReader.h"

class QTcpSocket;
class QTimer;
class BitSet;
class TorrentDownloader;

class PeerConnection
	:public QObject
{
	Q_OBJECT

private:
    File* mFile;
    QTcpSocket* socket;
    TorrentDownloader* peers;
    QString adress;
    quint16 port;
    std::string infoHash;
    std::string buffor;

    bool amchoked;
    bool peerchoked;
    bool aminterested;
    bool peerinterested;
    bool isInitialized;
public:
    PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, TorrentDownloader* parent);
    ~PeerConnection();

protected slots:
    void handshake();
    void choke();
    void unchoke();
    void interested();
    void not_interested();
    void have(int index);
    void bitfield(BitSet& pieces);
    void request(int index, int begin, int length);
    void piece(int index, int begin, std::string block);
    //void cancel();
    //void port();
    void readData();
signals:
    void pieceDownloaded(size_t index);
};
#endif