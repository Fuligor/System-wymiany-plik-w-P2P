#include "PeerConnection.h"
#include <QTcpSocket>
#include <QCryptographicHash>
#include <algorithm>
#include "TorrentDownloader.h"
#include "Client.h"

#include <qdebug.h>

PeerConnection::PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, BitSet& myPieces, TorrentDownloader* parent)
	:QObject(parent), socket(tcpSocket), /*isInitialized(false),*/ infoHash(infoHash), mFile(mFile), /*amchoked(true), aminterested(false), peerchoked(true), peerinterested(false),*/
	havePieces(mFile->getFragCount()), fragBuff(""), toDownload(0), isDownloading(false), download_index(0), myPieces(myPieces)
{
	connectTimer.setSingleShot(true);

	connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onDisconnection()));
	connect(&connectTimer, SIGNAL(timeout()), this, SLOT(onDisconnection()));
	connect(this, SIGNAL(pieceDownloaded(size_t)), parent, SLOT(onPieceDownloaded(size_t)));
	connect(parent, SIGNAL(pieceDownloaded(size_t)), this, SLOT(have(size_t)));
	connect(this, SIGNAL(initialize(std::string, PeerConnection*)), parent, SLOT(peerHandshake(std::string, PeerConnection*)));
	connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnection()));
	connect(this, SIGNAL(peerdisconnect(std::string, PeerConnection*)), parent, SLOT(closeConnection(std::string, PeerConnection*)));
	connect(this, SIGNAL(downloadRequest(PeerConnection*)), parent, SLOT(downloadMenager(PeerConnection*)));
	connect(this, SIGNAL(uploaded(size_t)), parent, SLOT(onPieceUploaded(size_t)));

	connectTimer.start(5000);

	connect(socket, SIGNAL(connected()), this, SLOT(handshake()));
	if (socket->state() == QAbstractSocket::ConnectedState)
	{
		handshake();
	}
}

PeerConnection::~PeerConnection()
{
	socket->disconnectFromHost();

	if(!socket->waitForDisconnected(5000))
	{
		socket->abort();
	}
}

void PeerConnection::downloadPiece(size_t index, size_t pieceSize, std::string fragHash)
{
	isDownloading = true;
	toDownload = pieceSize;
	expectedHash = fragHash;
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
std::string PeerConnection::write(size_t size)
{
	std::string result;
	for (size_t i = 0; i < sizeof(size_t); ++i)
	{
		unsigned char temp = size % 256;

		result += temp;

		size = size >> 8;
	}
	return result;
}
size_t PeerConnection::read(const std::string& size)
{
	size_t result = 0;

	for (size_t i = 0; i < sizeof(size_t); ++i)
	{
		unsigned char temp = (unsigned char)size[i];

		result = result + ((size_t)temp << (i * 8));
	}
	return result;
}

//void PeerConnection::choke()
//{
//	std::string message = "00010";
//	socket->write(message.data(), message.size());
//}
//
//void PeerConnection::unchoke()
//{
//	std::string message = "00011";
//	socket->write(message.data(), message.size());
//}
//
//void PeerConnection::interested()
//{
//	std::string message = "00012";
//	socket->write(message.data(), message.size());
//}
//
//void PeerConnection::not_interested()
//{
//	std::string message = "00013";
//	socket->write(message.data(), message.size());
//}

void PeerConnection::have(size_t index)
{
	std::string idx = write(index);

	std::string message = "4" + idx;
	message = write(message.size()) + message;

	socket->write(message.data(), message.size());

	if(havePieces.getCount() == havePieces.getSize() && myPieces.getCount() == myPieces.getSize())
	{
		socket->disconnectFromHost();
	}
}

void PeerConnection::bitfield()
{
	std::string message = "5" + std::string(myPieces.getData(), BitSet::getPageCount(myPieces.getSize()));
	message = write(message.size()) + message;

	socket->write(message.data(), message.size());
}

void PeerConnection::request(size_t index, size_t begin)
{	
	download_index = index;
	std::string idx = write(index);
	std::string beg = write(begin);
	std::string len = write(std::min(toDownload-fragBuff.size(), downloadLength));

	std::string message = "6" + idx + beg + len;
	message = write(message.size()) + message;
	socket->write(message.data(), message.size());
	socket->flush();
}

void PeerConnection::piece(size_t index, size_t begin, std::string block)
{	
	std::string idx = write(index);
	std::string beg = write(begin);

	std::string message = "7" + idx + beg + block;
	message = write(message.size()) + message;
	socket->write(message.data(), message.size());

	emit uploaded(block.size());
}

void PeerConnection::handshake()
{
	std::string message = "9" + infoHash + Client::getInstance()->getId();
	message = write(message.size()) + message;
	socket->write(message.data(), message.size());
}

void PeerConnection::readData()
{
	buffor += socket->readAll().toStdString();

	//qDebug() << QString::fromStdString(buffor);

	while (buffor.size() >= sizeof(size_t))
	{
		int message_size = read(buffor.substr(0, sizeof(size_t)));
		if (message_size + sizeof(size_t) <= buffor.size())
		{
			std::string message = buffor.substr(sizeof(size_t));
			/*if (message[0] == '0')
			{
				peerchoked = true;
			}
			else if (message[0] == '1')
			{
				peerchoked = false;
			}
			else if (message[0] == '2')
			{
				peerinterested = true;
			}
			else if (message[0] == '3')
			{
				peerinterested = false;
			}*/
			if (message[0] == '4')
			{
				havePieces.set(read(message.substr(1)));
				if (!isDownloading)
				{
					emit downloadRequest(this);
				}
			}
			else if (message[0] == '5')
			{
				havePieces.setData((unsigned char *)message.substr(1).c_str());
				if (!isDownloading)
				{
					emit downloadRequest(this);
				}
			}
			else if (message[0] == '6')
			{
				size_t index = read(message.substr(1, sizeof(size_t)));
				size_t begin = read(message.substr(1 + sizeof(size_t), sizeof(size_t)));
				size_t length = read(message.substr(1 + 2 * sizeof(size_t), sizeof(size_t)));
				piece(index, begin, (*mFile)[(const unsigned int)index]->getData().mid((int)begin, (int)length).toStdString());
				
			}
			else if (message[0] == '7')
			{
				size_t index = read(message.substr(1, sizeof(size_t)));
				//int begin = read(message.substr(1 + sizeof(size_t), sizeof(size_t)));

				if (index != download_index)
				{
					request(download_index, fragBuff.size());
				}
				else
				{
					std::string block = message.substr(1 + 2 * sizeof(size_t));
					fragBuff += block;
					if (fragBuff.size() == toDownload)
					{
						std::string hash = QCryptographicHash::hash(QByteArray::fromStdString(fragBuff), QCryptographicHash::Sha1).toStdString();

						if (hash == expectedHash)
						{
							mFile->setFrag(fragBuff, (int)index);
							fragBuff = "";
							isDownloading = false;
							emit pieceDownloaded(index);
							emit downloadRequest(this);
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
			else if (message[0] == '9')
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
			buffor = buffor.substr(message_size + sizeof(size_t));
		}
	}

}

void PeerConnection::onDisconnection()
{
	socket->abort();

	emit peerdisconnect(peerId, this);
}
