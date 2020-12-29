#ifndef TORRENT_DOWNLOADER_H
#define TORRENT_DOWNLOADER_H
#include <QObject>

#include <list>
#include <set>

#include "Bencode.h"

#include "Peer.h"
#include "TorrentDownloadStatus.h"

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
	TorrentDownloadStatus status;
	std::set <Peer> availablePeers;
	std::map <std::wstring, PeerConnection> connectedPeers;
public:
	TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, QObject* parent = nullptr);
	~TorrentDownloader();
	const TorrentDownloadStatus& getDownloadStatus() const;

protected slots:
	void updatePeerList(bencode::List peers);

signals:
	void peerAdded();
	void statusUpdated();
};
#endif // !TORRENT_DOWNLOADER_H
