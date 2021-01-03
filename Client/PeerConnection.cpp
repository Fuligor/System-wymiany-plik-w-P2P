#include "PeerConnection.h"
#include <QTcpSocket>
#include <algorithm>
#include "TorrentDownloader.h"
#include "Client.h"

PeerConnection::PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, TorrentDownloader* parent)
	:QObject(parent), socket(tcpSocket), /*isInitialized(false),*/ infoHash(infoHash), mFile(mFile), /*amchoked(true), aminterested(false), peerchoked(true), peerinterested(false),*/ 
	havePieces(mFile->getSize()), fragBuff(""), toDownload(0), isDownloading(false)
{
	
	connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(this, SIGNAL(pieceDownloaded(size_t)), parent, SLOT(onPieceDownloaded(size_t)));
	connect(parent, SIGNAL(pieceDownloaded(size_t)), this, SLOT(have(size_t)));
	connect(this, SIGNAL(initialize(std::string, PeerConnection*)), parent, SLOT(peerHandshake(std::string, PeerConnection*)));
	connect(socket, SIGNAL(disconnect()), this, SLOT(onDisconnection()));
	connect(this, SIGNAL(peerdisconnect(std::string, PeerConnection*)), parent, SLOT(closeConnection(std::string, PeerConnection*)));
	connect(this, SIGNAL(downloadRequest(PeerConnection*)), parent, SLOT(downloadMenager(PeerConnection*)));
}

PeerConnection::~PeerConnection()
{
}

void PeerConnection::downloadPiece(size_t index, size_t pieceSize, std::string fragHash)
{
	isDownloading = true;
	toDownload = pieceSize;
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
	for (size_t i = 0; i < sizeof(size); ++i)
	{
		char temp = size % 256;

		result += temp;

		size = size >> 8;
	}
	return result;
}
size_t PeerConnection::read(std::string& size)
{
	size_t result = 0;

	for (size_t i = 0; i < sizeof(size); ++i)
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

	std::string message = write(5) + "4" + idx;
	socket->write(message.data(), message.size());
}

void PeerConnection::bitfield(BitSet& pieces)
{

	size_t size = BitSet::getPageCount(pieces.getSize()) + 1;
	std::string message = write(size) + "5" + std::string(pieces.getData(), BitSet::getPageCount(pieces.getSize()));
	socket->write(message.data(), message.size());
}

void PeerConnection::request(size_t index, size_t begin)
{	
	std::string idx = write(index);
	std::string beg = write(begin);
	std::string len = write(std::min(toDownload-fragBuff.size(), downloadLength));

	std::string message = write(13) + "6" + idx + beg + len;
	socket->write(message.data(), message.size());
}

void PeerConnection::piece(size_t index, size_t begin, std::string block)
{	
	size_t blocksize = 9 + block.size();

	std::string size = write(blocksize);
	std::string idx = write(index);
	std::string beg = write(begin);


	std::string message = size + "7" + idx + beg + block;
	socket->write(message.data(), message.size());
}

void PeerConnection::handshake()
{
	std::string message = write(41) + "9" + infoHash + Client::getInstance()->getId();
	socket->write(message.data(), message.size());
}

void PeerConnection::readData()
{
	buffor += socket->readAll().toStdString();

	if (buffor.size() >= 4)
	{
		int message_size = read(buffor.substr(0, 4));
		if (message_size <= buffor.size() - 4)
		{
			std::string message = buffor.substr(5, message_size);
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
				size_t index = read(message.substr(1, 4));
				size_t begin = read(message.substr(5, 4));
				size_t length = read(message.substr(9, 4));
				piece(index, begin, (*mFile)[index]->getData().mid(begin, length).toStdString());
				
			}
			else if (message[0] == '7')
			{
				size_t index = read(message.substr(1, 4));
				//int begin = read(message.substr(5, 4));
				std::string block = message.substr(9);
				fragBuff += block;
				if (fragBuff.size() == toDownload)
				{
					mFile->setFrag(fragBuff, index);
					fragBuff = "";
					isDownloading = false;
					emit pieceDownloaded(index);
					emit downloadRequest(this);
					

				}
				else
				{
					request(index, fragBuff.size());
				}
				
			}
			else if (message[0] == '9')
			{
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
			buffor.erase(0, message_size + (size_t)4);
		}
	}

}

void PeerConnection::onDisconnection()
{
	emit peerdisconnect(peerId, this);
}
