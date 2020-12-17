#include "TorrentDownloader.h"

#include "Types.h"

#include "TrackerConnection.h"
#include "PeerConnection.h"

TorrentDownloader::TorrentDownloader(const std::string& fileName, QObject* parent)
	:QObject(parent), tracker(new TrackerConnection(fileName, this))
{
	connect(tracker, SIGNAL(peerListUpdated(bencode::List)), this, SLOT(updatePeerList(bencode::List)));
}

TorrentDownloader::~TorrentDownloader()
{
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