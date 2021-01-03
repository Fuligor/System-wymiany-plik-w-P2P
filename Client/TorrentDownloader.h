#ifndef TORRENT_DOWNLOADER_H
#define TORRENT_DOWNLOADER_H
#include <QObject>

#include <list>
#include <set>

#include "Bencode.h"

#include "Peer.h"
#include "File.h"
#include "TorrentDownloadStatus.h"
#include <qtcpserver.h>
#include <qtcpsocket.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include "BitSet.h"

class Torrent;
class TrackerConnection;
class PeerConnection;

namespace bencode
{
	class List;
}

class TorrentDownloader:
	public QObject
{
	Q_OBJECT

private:
	File* mFile;
	QMutex mutex;
	QWaitCondition condVar;
	QTcpServer tcpServer;
	TrackerConnection* tracker;
	TorrentDownloadStatus downloadStatus;
	std::string infoHash;
	std::set <Peer> availablePeers;
	std::set <std::string> connectedPeers;
	BitSet& pieces;
	BitSet piecesToDownload;
	size_t pieceSize;
	int activeConn = 0;
	int maxConn = 30;
	bool isAwaitingPeer;
public:
	TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, BitSet& pieces, Torrent* parent);
	~TorrentDownloader();
	const TorrentDownloadStatus& getDownloadStatus() const;
	void createConnection();
	void connectionManager();

protected:
	void calculateDownloadedSize();
	size_t getPieceSize(size_t index);

public slots:
	void onPieceDownloaded(size_t index);
	void onPieceUploaded(size_t index);
	void peerHandshake(std::string peerId, PeerConnection* connection);
	void closeConnection(std::string peerId, PeerConnection* connection);
	void downloadMenager(PeerConnection* connection);

protected slots:
	void updatePeerList(bencode::List peers);
	void onTrackerStatusChanged(const ConnectionStatus& status);
	void onNewConnection();
	

signals:
	void peerAdded();
	void statusUpdated();
	void pieceDownloaded(size_t index);
	void pieceUploaded(size_t index);
};
#endif // !TORRENT_DOWNLOADER_H
