#include "TorrentDownloader.h"

#include "Types.h"
#include "InfoDictHash.h"
#include "Torrent.h"
#include "TrackerConnection.h"
#include "PeerConnection.h"

TorrentDownloader::TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, BitSet& pieces, Torrent* parent)
	:QObject(parent), pieces(pieces), tcpServer(this), isAwaitingPeer(false), piecesToDownload(~pieces)
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

	tracker->connectToTracker();
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
	QString addres = QString::fromStdString(availablePeers.begin()->id);
	QTcpSocket* socket = new QTcpSocket(this);
	socket->connectToHost(addres, availablePeers.begin()->port);
	availablePeers.erase(availablePeers.begin());
	activeConn++;
	new PeerConnection(socket, infoHash, mFile, this);

}

void TorrentDownloader::connectionManager()
{
	if (activeConn < maxConn)
	{
		if (availablePeers.size() > 0)
		{
			createConnection();
		}
		else
		{
			tracker->updatePeerList();
		}
		
	}
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

		downloadStatus.downloadedSinceStart = piecesSize + getPieceSize(lastBit) * pieces.bit(lastBit);

		tracker->setLeft(downloadStatus.fileSize - downloadStatus.downloadedSinceStart);
	}
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

void TorrentDownloader::peerHandshake(std::string peerId, PeerConnection* connection)
{
	if (connectedPeers.find(peerId) == connectedPeers.end())
	{
		connectedPeers.insert(peerId);
	}
	else
	{
		delete connection;
		activeConn--;
		if (pieces.getSize() > pieces.getCount())
		{
			connectionManager();
		}
	}
	
	
}

void TorrentDownloader::closeConnection(std::string peerId, PeerConnection* connection)
{
	connectedPeers.erase(peerId);
	delete connection;
	activeConn--;
	if (pieces.getSize() > pieces.getCount())
	{
		connectionManager();
	}
}

void TorrentDownloader::downloadMenager(PeerConnection* connection)
{
	mutex.lock();
	if (connection->getIsDownloading())
	{
		mutex.unlock();
		return;
	}
	
	BitSet interestingPieces = piecesToDownload & connection->getPieces();
	size_t size = interestingPieces.getCount();
	if (size > 0)
	{
		size_t randomPiece = std::rand() % size + 1;
		size_t index = interestingPieces.getSetedBit(randomPiece);
		piecesToDownload.reset(index);
		connection->downloadPiece(index, getPieceSize(index), (*mFile)[index]->getHash().toStdString());
	}
	mutex.unlock();
	

}

void TorrentDownloader::onPieceDownloaded(size_t index)
{
	pieces.set(index);

	size_t pieceSize = getPieceSize(index);

	downloadStatus.downloaded += pieceSize;
	downloadStatus.downloadedSinceStart += pieceSize;

	tracker->setLeft(downloadStatus.fileSize - downloadStatus.downloadedSinceStart);
	tracker->setDownloaded(downloadStatus.downloaded);

	emit pieceDownloaded(index);
}

void TorrentDownloader::onPieceUploaded(size_t index)
{
	downloadStatus.uploaded += getPieceSize(index);

	tracker->setUploaded(downloadStatus.uploaded);

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
		if (connectedPeers.find(peer.id) == connectedPeers.end() && availablePeers.insert(peer).second)
		{
			connectionManager();
		}
		mutex.unlock();
	}
}