#ifndef TORRENT_DOWNLOADER_H
#define TORRENT_DOWNLOADER_H
#include <QObject>

#include <list>
#include <set>

#include "Bencode.h"

#include "Peer.h"
#include "TorrentDownloadStatus.h"

class BitSet;
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
	TrackerConnection* tracker;
	TorrentDownloadStatus downloadStatus;
	std::set <Peer> availablePeers;
	std::map <std::wstring, PeerConnection> connectedPeers;
	BitSet& pieces;
	size_t pieceSize;
public:
	TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, BitSet& pieces, Torrent* parent);
	~TorrentDownloader();
	const TorrentDownloadStatus& getDownloadStatus() const;

protected:
	void calculateDownloadedSize();
	size_t getPieceSize(size_t index);

public slots:
	void onPieceDownloaded(size_t index);
	void onPieceUploaded(size_t index);

protected slots:
	void updatePeerList(bencode::List peers);
	void onTrackerStatusChanged(const ConnectionStatus& status);

signals:
	void peerAdded();
	void statusUpdated();
	void pieceDownloaded(size_t index);
	void pieceUploaded(size_t index);
};
#endif // !TORRENT_DOWNLOADER_H
