#include "PeerConnection.h"
#include <QTcpSocket>
#include <QCryptographicHash>
#include <algorithm>
#include "TorrentDownloader.h"
#include "Client.h"

#include <qdebug.h>

PeerConnection::PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, BitSet& myPieces, TorrentDownloader* parent)
	:QObject(parent), mFile(mFile), socket(tcpSocket), infoHash(infoHash), myPieces(myPieces),
	havePieces(mFile->getFragCount()), fragBuff(""), toDownload(0), isDownloading(false), download_index(0), retransmisions(0)
{
	connectTimer.setSingleShot(true);

	socket->setParent(this);
	socket->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption, true);
	
	connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(&connectTimer, SIGNAL(timeout()), this, SLOT(onDisconnection()));
	connect(this, SIGNAL(pieceDownloaded(uint64_t)), parent, SLOT(onPieceDownloaded(uint64_t)));
	connect(parent, SIGNAL(pieceDownloaded(uint64_t)), this, SLOT(have(uint64_t)));
	connect(this, SIGNAL(initialize(std::string, PeerConnection*)), parent, SLOT(peerHandshake(std::string, PeerConnection*)));
	connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onDisconnection()));
	connect(this, SIGNAL(peerdisconnect(std::string, PeerConnection*)), parent, SLOT(closeConnection(std::string, PeerConnection*)));
	connect(this, SIGNAL(downloadRequest(PeerConnection*)), parent, SLOT(downloadMenager(PeerConnection*)));
	connect(this, SIGNAL(uploaded(uint64_t)), parent, SLOT(onPieceUploaded(uint64_t)));
	connect(parent, SIGNAL(updateStatistics()), this, SLOT(updateStatistics()));
	connect(this, SIGNAL(downloadCanceled(uint64_t)), parent, SLOT(onDownloadCanceled(uint64_t)));
	connect(this, SIGNAL(speedUpdated(FileSize, FileSize, FileSize)), 
			parent, SLOT(speedUpdated(FileSize, FileSize, FileSize)));

	connectTimer.start(5000);

	connect(socket, SIGNAL(connected()), this, SLOT(handshake()));
	if (socket->state() == QAbstractSocket::ConnectedState)
	{
		handshake();
	}
}

PeerConnection::~PeerConnection()
{
	updateStatistics();

	if(isDownloading)
	{
		downloadCanceled(download_index);
	}

	socket->disconnectFromHost();

	if (socket->state() == QAbstractSocket::ConnectedState)
	{
		if (!socket->waitForDisconnected(5000))
		{
			socket->abort();
		}
	}
}

void PeerConnection::downloadPiece(uint64_t index, uint64_t pieceSize, std::string fragHash)
{
	isDownloading = true;
	toDownload = pieceSize;
	expectedHash = fragHash;
	retransmisions = 0;
	request(index, 0);
}
const BitSet& PeerConnection::getPieces()
{
	return havePieces;
}
bool PeerConnection::getIsDownloading()
{
	return isDownloading;
}
std::string PeerConnection::write(uint64_t size)
{
	std::string result;
	for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
	{
		unsigned char temp = size % 256;

		result += temp;

		size = size >> 8;
	}
	return result;
}

uint64_t PeerConnection::read(const std::string& size)
{
	uint64_t result = 0;

	for (uint64_t i = 0; i < sizeof(uint64_t); ++i)
	{
		unsigned char temp = (unsigned char) size[i];

		result = result + ((uint64_t) temp << (i * 8));
	}
	return result;
}

void PeerConnection::handshake()
{
	std::string message = "0" + infoHash + Client::getInstance()->getId();
	message = write(message.size()) + message;
	socket->write(message.data(), message.size());
	socket->flush();

	statisticsMutex.lock();
	uploadedSinceLastUpdate += message.size();
	statisticsMutex.unlock();
}

void PeerConnection::bitfield()
{
	std::string message = "1" + std::string(myPieces.getData(), BitSet::getPageCount(myPieces.getSize()));
	message = write(message.size()) + message;

	socket->write(message.data(), message.size());
	socket->flush();

	statisticsMutex.lock();
	uploadedSinceLastUpdate += message.size();
	statisticsMutex.unlock();
}


void PeerConnection::have(uint64_t index)
{
	std::string idx = write(index);

	std::string message = "2" + idx;
	message = write(message.size()) + message;

	socket->write(message.data(), message.size());
	socket->flush();

	statisticsMutex.lock();
	uploadedSinceLastUpdate += message.size();
	statisticsMutex.unlock();

	if (havePieces.getCount() == havePieces.getSize() && myPieces.getCount() == myPieces.getSize())
	{
		socket->disconnectFromHost();
	}
}

void PeerConnection::request(uint64_t index, uint64_t begin)
{
	download_index = index;
	std::string idx = write(index);
	std::string beg = write(begin);
	std::string len = write(std::min(toDownload - fragBuff.size(), downloadLength));

	std::string message = "3" + idx + beg + len;
	message = write(message.size()) + message;
	socket->write(message.data(), message.size());
	socket->flush();

	statisticsMutex.lock();
	uploadedSinceLastUpdate += message.size();
	statisticsMutex.unlock();
}

void PeerConnection::piece(uint64_t index, uint64_t begin, std::string block)
{
	std::string idx = write(index);
	std::string beg = write(begin);

	std::string message = "4" + idx + beg + block;
	message = write(message.size()) + message;
	socket->write(message.data(), message.size());
	socket->flush();

	statisticsMutex.lock();
	uploadedSinceLastUpdate += message.size();
	statisticsMutex.unlock();

	emit uploaded(block.size());
}

void PeerConnection::readData()
{
	uint64_t readedBytes = buffor.size();
	buffor += socket->readAll().toStdString();
	
	readedBytes = buffor.size() - readedBytes;

	statisticsMutex.lock();
	downloadedSinceLastUpdate += readedBytes;
	statisticsMutex.unlock();

	while (buffor.size() >= sizeof(uint64_t))
	{
		int message_size = read(buffor.substr(0, sizeof(uint64_t)));
		if (message_size + sizeof(uint64_t) <= buffor.size())
		{
			std::string message = buffor.substr(sizeof(uint64_t), message_size);
			
			if (message[0] == '0')
			{
				connectTimer.stop();

				std::string hash = message.substr(1, 20);
				peerId = message.substr(21);
				if (hash != infoHash)
				{
					socket->disconnectFromHost();
				}
				else
				{
					emit initialize(peerId, this);
				}
			}
			else if (message[0] == '1')
			{
				havePieces.setData((unsigned char*) message.substr(1).c_str());
				if (!isDownloading)
				{
					emit downloadRequest(this);
				}
			}
			else if (message[0] == '2')
			{
				havePieces.set(read(message.substr(1)));
				if (!isDownloading)
				{
					emit downloadRequest(this);
				}
			}
			else if (message[0] == '3')
			{
				uint64_t index = read(message.substr(1, sizeof(uint64_t)));
				int begin = read(message.substr(1 + sizeof(uint64_t), sizeof(uint64_t)));
				int length = read(message.substr(1 + 2 * sizeof(uint64_t), sizeof(uint64_t)));
				piece(index, begin, (*mFile)[(const unsigned int) index]->getData().mid((int) begin, (int) length).toStdString());
			}
			else if (message[0] == '4')
			{
				uint64_t index = read(message.substr(1, sizeof(uint64_t)));
				//uint64_t begin = read(message.substr(1 + sizeof(uint64_t), sizeof(uint64_t)));

				if (index != download_index)
				{
					request(download_index, fragBuff.size());
				}
				else
				{
					std::string block = message.substr(1 + 2 * sizeof(uint64_t));

					statisticsMutex.lock();
					fileDownloadSpeed += block.size();
					statisticsMutex.unlock();

					fragBuff += block;
					if (fragBuff.size() == toDownload)
					{
						std::string hash = QCryptographicHash::hash(QByteArray::fromStdString(fragBuff), QCryptographicHash::Sha1).toStdString();

						if (hash == expectedHash)
						{
							mFile->setFrag(fragBuff, (int) index);
							fragBuff = "";
							isDownloading = false;
							emit pieceDownloaded(index);
							emit downloadRequest(this);
						}
						else if(retransmisions++ > 3)
						{
							fragBuff = "";
							isDownloading = false;
							havePieces.reset(download_index);
							emit downloadCanceled(index);
						}
						else
						{
							fragBuff = "";
							request(index, fragBuff.size());
						}
					}
					else
					{
						request(index, fragBuff.size());
					}
				}
			}

			buffor = buffor.substr(message_size + sizeof(uint64_t));
		}
		else
		{
			break;
		}
	}

}

void PeerConnection::onDisconnection()
{
	if (socket->state() == QAbstractSocket::ConnectedState)
	{
		socket->abort();

		socket->deleteLater();
	}

	if(isDownloading)
	{
		emit downloadCanceled(download_index);
	}

	emit peerdisconnect(peerId, this);
}

void PeerConnection::updateStatistics()
{
	statisticsMutex.lock();

	emit speedUpdated(downloadedSinceLastUpdate, uploadedSinceLastUpdate, fileDownloadSpeed);

	downloadedSinceLastUpdate = 0;
	uploadedSinceLastUpdate = 0;
	fileDownloadSpeed = 0;

	statisticsMutex.unlock();
}
