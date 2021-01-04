#include "TorrentDownloader.h"

#include "Types.h"
#include "InfoDictHash.h"
#include "Torrent.h"
#include "TrackerConnection.h"
#include "PeerConnection.h"

#include <QDebug>

TorrentDownloader::TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, TorrentConfig& status, Torrent* parent)
	:QObject(parent), pieces(status.pieceStatus), tcpServer(this), isAwaitingPeer(false), piecesToDownload(~pieces)
{
	std::shared_ptr <bencode::Dict> info = std::dynamic_pointer_cast <bencode::Dict> ((*torrentDict)["info"]);
	infoHash = InfoDictHash::getHash(torrentDict);

	connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	connect(this, SIGNAL(pieceDownloaded(size_t)), this, SIGNAL(statusUpdated()));
	connect(this, SIGNAL(pieceUploaded(size_t)), this, SIGNAL(statusUpdated()));

	tcpServer.listen();
	tracker = new TrackerConnection(torrentDict, tcpServer.serverPort(), this);
	pieceSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["piece length"])->getValue();
	
	downloadStatus.fileName = std::dynamic_pointer_cast <bencode::String> ((*info)["name"])->data();
	downloadStatus.fileSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["length"])->getValue();
	downloadStatus.connectionCount = 0;
	mFile = new File(QString::fromStdString(status.downloadPath), pieceSize, downloadStatus.fileSize.to_int());

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
	QString addres = QString::fromStdString(availablePeers.begin()->address);
	QTcpSocket* socket = new QTcpSocket(this);

	//qDebug() << addres << " " << availablePeers.begin()->port;

	socket->connectToHost(addres, availablePeers.begin()->port);
	availablePeers.erase(availablePeers.begin());
	downloadStatus.connectionCount++;

	new PeerConnection(socket, infoHash, mFile, this);

	emit statusUpdated();
}

void TorrentDownloader::connectionManager()
{
	while (downloadStatus.connectionCount < maxConn)
	{
		if (availablePeers.size() > 0)
		{
			createConnection();
		}
		else
		{
			tracker->updatePeerList();
			break;
		}
	}
}

size_t TorrentDownloader::getPieceSize(size_t index)
{
	if (index == pieces.getSize() - 1)
	{
		return downloadStatus.fileSize.to_int() - (pieces.getSize() - 1) * pieceSize;
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

		tracker->setLeft(downloadStatus.fileSize.to_int() - downloadStatus.downloadedSinceStart.to_int());
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
		if (downloadStatus.downloadedSinceStart.to_int() == downloadStatus.fileSize.to_int())
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

		connection->bitfield(pieces);
	}
	else
	{
		delete connection;
		downloadStatus.connectionCount--;

		if (pieces.getSize() > pieces.getCount())
		{
			connectionManager();
		}

		emit statusUpdated();
	}
	
	
}

void TorrentDownloader::closeConnection(std::string peerId, PeerConnection* connection)
{
	connectedPeers.erase(peerId);
	delete connection;
	downloadStatus.connectionCount--;
	if (pieces.getSize() > pieces.getCount())
	{
		connectionManager();
	}

	emit statusUpdated();
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

	tracker->setLeft(downloadStatus.fileSize.to_int() - downloadStatus.downloadedSinceStart.to_int());
	tracker->setDownloaded(downloadStatus.downloaded.to_int());

	if(downloadStatus.downloadedSinceStart.to_int() == downloadStatus.fileSize.to_int())
	{
		downloadStatus.connectionState == TorrentDownloadStatus::State::SEEDING;

		tracker->completeRequest();
	}
	else
	{
		downloadStatus.connectionState == TorrentDownloadStatus::State::LEECHING;
	}

	emit pieceDownloaded(index);
}

void TorrentDownloader::onPieceUploaded(size_t pieceSize)
{
	downloadStatus.uploaded += pieceSize;

	tracker->setUploaded(downloadStatus.uploaded.to_int());

	emit pieceUploaded(pieceSize);
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