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
#include "FileFrag.h"
#include "torrentReader.h"
#include "BitSet.h"
class QTcpSocket;
class TorrentDownloader;

class PeerConnection
	:public QObject
{
	Q_OBJECT

private:
    File* mFile;
    QTcpSocket* socket;
    std::string peerId;
    std::string infoHash;
    std::string buffor;
    BitSet havePieces;
    std::string fragBuff;
    size_t toDownload;
    size_t downloadLength = 16 * 1024;
    bool isDownloading;
    /*bool amchoked;
    bool peerchoked;
    bool aminterested;
    bool peerinterested;
    bool isInitialized;*/
public:
    PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, TorrentDownloader* parent);
    ~PeerConnection();
    void downloadPiece(size_t index, size_t pieceSize, std::string fragHash);
    const BitSet& getPieces();
    bool getIsDownloading();
protected:
    std::string write(size_t size);
    size_t read(std::string& size);
protected slots:
    void handshake();
    /*void choke();
    void unchoke();
    void interested();
    void not_interested();*/
    void have(size_t index);
    void bitfield(BitSet& pieces);
    void request(size_t index, size_t begin);
    void piece(size_t index, size_t begin, std::string block);
    void readData();
    void onDisconnection();
signals:
    void downloadRequest(PeerConnection* conn);
    void pieceDownloaded(size_t index);
    void initialize(std::string idpeer, PeerConnection* conn);
    void peerdisconnect(std::string peerid, PeerConnection* conn);
};
#endif