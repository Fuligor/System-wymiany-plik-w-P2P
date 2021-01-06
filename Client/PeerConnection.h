//Klasa odpowiedzialna za nawi¹zywanie po³¹czeñ i komunikacjê miêdzy klientami
//Opis komunikacji znajduje siê w sprawozdaniu
#ifndef PEER_CONNECTION_H
#define PEER_CONNECTION_H

#include <QObject>
#include <set>
#include <list>

#include <QString>
#include <QTimer>
#include <QMutex>

#include "Types.h"
#include "Bencode.h"
#include "ConnectionStatus.h"
#include "Peer.h"
#include "File.h"
#include "FileFrag.h"
#include "FileSize.h"
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
    QTimer connectTimer;
    QMutex statisticsMutex;

    std::string peerId;
    std::string infoHash;
    std::string buffor;
    BitSet& myPieces;
    BitSet havePieces;
    std::string fragBuff;
    uint64_t toDownload;
    //Maksymalny rozmiar bloku fragmentu jednorazowo przesy³anego przez socket
    uint64_t downloadLength = 16 * 1024;
    bool isDownloading;
    std::string expectedHash;
    uint64_t download_index;
    uint64_t retransmisions;

    FileSize downloadedSinceLastUpdate;
    FileSize uploadedSinceLastUpdate;
    FileSize fileDownloadSpeed;
public:
    PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, BitSet& myPieces, TorrentDownloader* parent);
    ~PeerConnection();
    void downloadPiece(uint64_t index, uint64_t pieceSize, std::string fragHash);
    //zwraca fragmenty posiadane przez druga stronê
    const BitSet& getPieces();
    bool getIsDownloading();
protected:
    //zapisanie liczby w postaci bajtów
    std::string write(uint64_t size);
    //odczytanie bajtów do postaci liczby
    uint64_t read(const std::string& size);
public slots:
    void bitfield();
protected slots:
    void handshake();
    void have(uint64_t index);
    void request(uint64_t index, uint64_t begin);
    void piece(uint64_t index, uint64_t begin, std::string block);
    //Obs³uga otrzymanych wiadomoœci
    void readData();
    void onDisconnection();
    void updateStatistics();
signals:
    void downloadRequest(PeerConnection* conn);
    void pieceDownloaded(uint64_t index);
    void initialize(std::string idpeer, PeerConnection* conn);
    void peerdisconnect(std::string peerid, PeerConnection* conn);
    void uploaded(uint64_t uploadSize);
    void speedUpdated(FileSize, FileSize, FileSize);
    void downloadCanceled(uint64_t index);
};
#endif