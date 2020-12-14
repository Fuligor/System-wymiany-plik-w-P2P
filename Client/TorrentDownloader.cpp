#include "TorrentDownloader.h"

#include "Types.h"

#include "TrackerConnection.h"




#include <iostream>

TorrentDownloader::TorrentDownloader(const std::string& fileName)
	:tracker(new TrackerConnection(fileName))
{
	connect(tracker, SIGNAL(peerListUpdated), this, SLOT(updatePeerList));
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

		peer.port = dynamic_cast <bencode::Int*> (peerDict["peer id"].get())->getValue();

		std::cout << peer.id << " " << peer.address << " " << peer.port << std::endl;

		if(availablePeers.insert(peer).second)
		{
			emit peerAdded();
		}
	}
}