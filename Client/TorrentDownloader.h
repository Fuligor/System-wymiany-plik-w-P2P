#ifndef TORRENT_DOWNLOADER_H
#define TORRENT_DOWNLOADER_H
#include <QObject>

#include <list>
#include <set>

#include <QTimer>

#include "Bencode.h"

#include "ConnectionStatus.h"
#include "Peer.h"
#include "File.h"
#include "TorrentConfig.h"
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
	QMutex statusMutex;
	QWaitCondition condVar;
	QTcpServer tcpServer;
	TrackerConnection* tracker;
	TorrentDownloadStatus downloadStatus;
	std::string infoHash;
	std::string piecesHash;
	std::set <Peer> availablePeers;
	std::set <std::string> connectedPeers;
	BitSet& pieces;
	BitSet piecesToDownload;
	uint64_t pieceSize;
	uint64_t maxConn = 30;
	FileSize downloadSpeed;

	QTimer updateTimer;
public:
	TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, TorrentConfig& status, Torrent* parent);
	~TorrentDownloader();
	const TorrentDownloadStatus& getDownloadStatus() const;
	void createConnection();
	void connectionManager();

protected:
	void calculateDownloadedSize();
	uint64_t getPieceSize(uint64_t index);

public slots:
	void onPieceDownloaded(uint64_t index);
	void onPieceUploaded(uint64_t index);
	void peerHandshake(std::string peerId, PeerConnection* connection);
	void closeConnection(std::string peerId, PeerConnection* connection);
	void downloadMenager(PeerConnection* connection);
	void speedUpdated(FileSize newDownload, FileSize newUpload, FileSize newFileDownload);
	void onDownloadCanceled(uint64_t index);

protected slots:
	void updatePeerList(bencode::List peers);
	void onTrackerStatusChanged(const ConnectionStatus& status);
	void onNewConnection();
	void onUpdateTimeout();
	
signals:
	void peerAdded();
	void statusUpdated();
	void pieceDownloaded(uint64_t index);
	void pieceUploaded(uint64_t pieceSize);
	void updateStatistics();
};
#endif // !TORRENT_DOWNLOADER_H
