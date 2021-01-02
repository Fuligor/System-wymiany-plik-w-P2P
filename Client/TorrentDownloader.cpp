#include "TorrentDownloader.h"

#include "Types.h"
#include "InfoDictHash.h"
#include "Torrent.h"
#include "TrackerConnection.h"
#include "PeerConnection.h"

TorrentDownloader::TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, BitSet& pieces, Torrent* parent)
	:QObject(parent), pieces(pieces), tcpServer(this), isAwaitingPeer(false)
{
	std::shared_ptr <bencode::Dict> info = std::dynamic_pointer_cast <bencode::Dict> ((*torrentDict)["info"]);
	infoHash = InfoDictHash::getHash(torrentDict);
	tcpServer.listen();
	tracker = new TrackerConnection(torrentDict, tcpServer.serverPort(), this);
	pieceSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["piece length"])->getValue();
	
	downloadStatus.fileName = std::dynamic_pointer_cast <bencode::String> ((*info)["name"])->data();
	downloadStatus.fileSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["length"])->getValue();
	mFile = new File(QString::fromStdWString(downloadStatus.fileName), pieceSize, downloadStatus.fileSize);
	connect(this, SIGNAL(pieceDownloaded(size_t)), this, SIGNAL(statusUpdated()));
	connect(this, SIGNAL(pieceUploaded(size_t)), this, SIGNAL(statusUpdated()));

	calculateDownloadedSize();
}

TorrentDownloader::~TorrentDownloader()
{
	delete tracker;
}

const TorrentDownloadStatus& TorrentDownloader::getDownloadStatus() const
{
	return downloadStatus;
}

void TorrentDownloader::createConnection()
{
	mutex.lock();
	if (availablePeers.empty())
	{
		isAwaitingPeer = true;
		condVar.wait(&mutex);
	}
	QString addres = QString::fromStdString(availablePeers.begin()->id);
	QTcpSocket* socket = new QTcpSocket(this);
	socket->connectToHost(addres, availablePeers.begin()->port);
	availablePeers.erase(availablePeers.begin());
	mutex.unlock();
	new PeerConnection(socket, infoHash, mFile, this);

}

size_t TorrentDownloader::getPieceSize(size_t index)
{
	if (index == pieces.getSize() - 1)
	{
		return downloadStatus.fileSize - (pieces.getSize() - 1) * pieceSize;
	}

	return pieceSize;
}

void TorrentDownloader::calculateDownloadedSize()
{
	if (pieces.getSize() == 0)
	{
		downloadStatus.downloadedSinceStart = 0;
	}
	else
	{
		size_t lastBit = pieces.getSize() - 1;

		size_t piecesSize = (pieces.getCount() - pieces.bit(lastBit)) * pieceSize;

		std::cout << piecesSize << " " << pieces.bit(lastBit) << std::endl;

		downloadStatus.downloadedSinceStart = piecesSize + getPieceSize(lastBit) * pieces.bit(lastBit);
	}

	std::cout << downloadStatus.downloadedSinceStart << std::endl;
}

void TorrentDownloader::onTrackerStatusChanged(const ConnectionStatus& status)
{
	switch (status)
	{
	case ConnectionStatus::INIT:
		downloadStatus.connectionState = TorrentDownloadStatus::State::CONNECTING_TO_TRACKER;
		break;
	case ConnectionStatus::REFUSED:
		downloadStatus.connectionState = TorrentDownloadStatus::State::TRACKER_CONNECTION_REFUSED;
		break;
	case ConnectionStatus::CLOSED:
		downloadStatus.connectionState = TorrentDownloadStatus::State::CLOSED;
		break;
	default:
		if (downloadStatus.downloadedSinceStart == downloadStatus.fileSize)
		{
			downloadStatus.connectionState = TorrentDownloadStatus::State::SEEDING;
		}
		else
		{
			downloadStatus.connectionState = TorrentDownloadStatus::State::LEECHING;
		}
		break;
	}

	emit statusUpdated();
}

void TorrentDownloader::onNewConnection()
{
	new PeerConnection(tcpServer.nextPendingConnection(), infoHash, mFile, this);

}

void TorrentDownloader::peerHandshake(std::string peerId)
{

	
}

void TorrentDownloader::onPieceDownloaded(size_t index)
{
	pieces.set(index);

	downloadStatus.downloaded += getPieceSize(index);
	downloadStatus.downloadedSinceStart += getPieceSize(index);

	emit pieceDownloaded(index);
}

void TorrentDownloader::onPieceUploaded(size_t index)
{
	downloadStatus.uploaded += getPieceSize(index);

	emit pieceUploaded(index);
}

void TorrentDownloader::updatePeerList(bencode::List peers)
{
	for (auto i : peers)
	{
		bencode::Dict peerDict = *(dynamic_cast <bencode::Dict*> (i.get()));
		Peer peer;

		std::wstring buf = dynamic_cast <bencode::String*> (peerDict["peer id"].get())->c_str();
		peer.id = std::string(buf.begin(), buf.end());

		buf = dynamic_cast <bencode::String*> (peerDict["ip"].get())->c_str();
		peer.address = std::string(buf.begin(), buf.end());

		peer.port = dynamic_cast <bencode::Int*> (peerDict["port"].get())->getValue();
		mutex.lock();
		if (availablePeers.insert(peer).second)
		{
			if (isAwaitingPeer)
			{
				isAwaitingPeer = false;
				condVar.wakeOne();
			}
		}
		mutex.unlock();
	}
}