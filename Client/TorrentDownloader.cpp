#include "TorrentDownloader.h"

#include "Types.h"
#include "InfoDictHash.h"
#include "Torrent.h"
#include "TrackerConnection.h"
#include "PeerConnection.h"

#include <QDebug>

TorrentDownloader::TorrentDownloader(const std::shared_ptr <bencode::Dict>& torrentDict, TorrentConfig& status, Torrent* parent)
	:QObject(parent), tcpServer(this), pieces(status.pieceStatus), piecesToDownload(~pieces)
{
	std::shared_ptr <bencode::Dict> info = std::dynamic_pointer_cast <bencode::Dict> ((*torrentDict)["info"]);
	infoHash = InfoDictHash::getHash(torrentDict);

	connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimeout()));

	tcpServer.listen();
	tracker = new TrackerConnection(torrentDict, tcpServer.serverPort(), this);
	pieceSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["piece length"])->getValue();
	std::shared_ptr <bencode::String> temp = std::dynamic_pointer_cast <bencode::String> ((*info)["pieces"]);
	
	for (uint64_t i = 0; i < temp->size(); ++i)
	{
		piecesHash += (char) temp->at(i);
	}
	
	downloadStatus.startTime.start();
	downloadStatus.fileName = std::dynamic_pointer_cast <bencode::String> ((*info)["name"])->data();
	downloadStatus.fileSize = std::dynamic_pointer_cast <bencode::Int> ((*info)["length"])->getValue();
	downloadStatus.connectionCount = 0;
	downloadStatus.estimatedEndTime.setHMS(99, 59, 59);

	updateTimer.setInterval(1000);
	updateTimer.setSingleShot(false);
	updateTimer.start();

	mFile = new File(QString::fromStdString(status.downloadPath), pieceSize, downloadStatus.fileSize.to_int());

	calculateDownloadedSize();

	tracker->connectToTracker();
}

TorrentDownloader::~TorrentDownloader()
{
}

const TorrentDownloadStatus& TorrentDownloader::getDownloadStatus() const
{
	return downloadStatus;
}

void TorrentDownloader::createConnection()
{
	QString addres = QString::fromStdString(availablePeers.begin()->address);
	QTcpSocket* socket = new QTcpSocket();

	socket->connectToHost(addres, availablePeers.begin()->port);
	availablePeers.erase(availablePeers.begin());

	statusMutex.lock();
	downloadStatus.connectionCount++;
	statusMutex.unlock();

	new PeerConnection(socket, infoHash, mFile, pieces, this);

	emit connectionUpated();
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

uint64_t TorrentDownloader::getPieceSize(uint64_t index)
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
		uint64_t lastBit = pieces.getSize() - 1;

		uint64_t piecesSize = (pieces.getCount() - pieces.bit(lastBit)) * pieceSize;

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
			statusMutex.lock();
			downloadStatus.connectionState = TorrentDownloadStatus::State::SEEDING;
			statusMutex.unlock();
			mFile->readonly(true);
		}
		else
		{
			statusMutex.lock();
			downloadStatus.connectionState = TorrentDownloadStatus::State::LEECHING;
			statusMutex.unlock();
			mFile->readonly(false);
		}
		break;
	}

	emit statusUpdated();
}

void TorrentDownloader::onNewConnection()
{
	downloadStatus.connectionCount++;

	new PeerConnection(tcpServer.nextPendingConnection(), infoHash, mFile, pieces, this);
}

void TorrentDownloader::onUpdateTimeout()
{
	downloadStatus.downloadSpeed = 0;
	downloadStatus.uploadSpeed = 0;
	downloadSpeed = 0;

	emit updateStatistics();
	emit speedUpdated();
}

void TorrentDownloader::peerHandshake(std::string peerId, PeerConnection* connection)
{
	if (connectedPeers.find(peerId) == connectedPeers.end())
	{
		connectedPeers.insert(peerId);

		connection->bitfield();
	}
	else
	{
		connection->deleteLater();

		statusMutex.lock();
		downloadStatus.connectionCount--;
		statusMutex.unlock();

		if (pieces.getSize() > pieces.getCount())
		{
			connectionManager();
		}

		emit connectionUpated();
	}
}

void TorrentDownloader::closeConnection(std::string peerId, PeerConnection* connection)
{
	statusMutex.lock();
	downloadStatus.connectionCount -= connectedPeers.erase(peerId);
	statusMutex.unlock();
	
	connection->deleteLater();
	if (pieces.getSize() > pieces.getCount())
	{
		connectionManager();
	}

	emit connectionUpated();
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
	uint64_t size = interestingPieces.getCount();
	if (size > 0)
	{
		uint64_t randomPiece = std::rand() % size + 1;
		uint64_t index = interestingPieces.getSetedBit(randomPiece);
		piecesToDownload.reset(index);
		connection->downloadPiece(index, getPieceSize(index), piecesHash.substr(index * 20, 20));
	}
	mutex.unlock();
}

void TorrentDownloader::updateSpeed(FileSize newDownload, FileSize newUpload, FileSize newFileDownload)
{
	statusMutex.lock();

	downloadStatus.downloadSpeed += newDownload;
	downloadStatus.uploadSpeed += newUpload;
	downloadSpeed += newFileDownload;

	FileSize left = downloadStatus.fileSize - downloadStatus.downloadedSinceStart;

	if (downloadSpeed.to_int() > 0)
	{
		downloadStatus.estimatedEndTime = QTime::fromMSecsSinceStartOfDay(left.to_int() * 1000 / downloadSpeed.to_int());
	}
	else
	{
		downloadStatus.estimatedEndTime.setHMS(99, 59, 59);
	}

	statusMutex.unlock();

	emit speedUpdated();
}

void TorrentDownloader::onDownloadCanceled(uint64_t index)
{
	mutex.lock();
	piecesToDownload.set(index);
	mutex.unlock();

	return;
}

void TorrentDownloader::onPieceDownloaded(uint64_t index)
{
	pieces.set(index);

	uint64_t pieceSize = getPieceSize(index);

	downloadStatus.downloaded += pieceSize;
	downloadStatus.downloadedSinceStart += pieceSize;

	tracker->setLeft(downloadStatus.fileSize.to_int() - downloadStatus.downloadedSinceStart.to_int());

	if(downloadStatus.downloadedSinceStart.to_int() == downloadStatus.fileSize.to_int())
	{
		downloadStatus.connectionState = TorrentDownloadStatus::State::SEEDING;

		tracker->completeRequest();
	}
	else
	{
		downloadStatus.connectionState = TorrentDownloadStatus::State::LEECHING;
	}

	emit pieceDownloaded(index);
}

void TorrentDownloader::onPieceUploaded(uint64_t pieceSize)
{
	downloadStatus.uploaded += pieceSize;

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