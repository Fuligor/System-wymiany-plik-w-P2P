#include "TorrentDownloader.h"

#include "Types.h"

#include "TrackerConnection.h"
#include "PeerConnection.h"

TorrentDownloader::TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, QObject* parent)
	:QObject(parent), tracker(new TrackerConnection(torrentDict, this))
{
	connect(tracker, SIGNAL(peerListUpdated(bencode::List)), this, SLOT(updatePeerList(bencode::List)));

	std::shared_ptr <bencode::Dict> info = std::dynamic_pointer_cast <bencode::Dict> ((*torrentDict)["info"]);

	status.fileName = std::dynamic_pointer_cast <bencode::String> ((*info)["name"])->data();
	status.fileSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["length"])->getValue();
}

TorrentDownloader::~TorrentDownloader()
{
}

const TorrentDownloadStatus& TorrentDownloader::getDownloadStatus() const
{
	return status;
}

void TorrentDownloader::updatePeerList(bencode::List peers)
{
	for(auto i: peers)
	{
		bencode::Dict peerDict = *(dynamic_cast <bencode::Dict*> (i.get()));
		Peer peer;

		std::wstring buf = dynamic_cast <bencode::String*> (peerDict["peer id"].get())->c_str();
		peer.id = std::string(buf.begin(), buf.end());

		buf = dynamic_cast <bencode::String*> (peerDict["ip"].get())->c_str();
		peer.address = std::string(buf.begin(), buf.end());

		peer.port = dynamic_cast <bencode::Int*> (peerDict["port"].get())->getValue();

		if(availablePeers.insert(peer).second)
		{
			emit peerAdded();
		}
	}
}