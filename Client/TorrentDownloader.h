#ifndef TORRENT_DOWNLOADER_H
#define TORRENT_DOWNLOADER_H
#include <QObject>

#include <list>
#include <set>

#include "Peer.h"

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
	std::set <Peer> availablePeers;
	//std::list <PeerConnection> connectedPeers;

public:
	TorrentDownloader(const std::string& fileName, QObject* parent = nullptr);
	~TorrentDownloader();

protected slots:
	void updatePeerList(bencode::List peers);

signals:
	void peerAdded();
};
#endif // !TORRENT_DOWNLOADER_H
