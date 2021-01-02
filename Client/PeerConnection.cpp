#include "PeerConnection.h"
#include <QTcpSocket>
#include <algorithm>
#include "TorrentDownloader.h"
#include "Client.h"
PeerConnection::PeerConnection(QTcpSocket* tcpSocket, std::string infoHash, File* mFile, TorrentDownloader* parent)
	:QObject(parent), socket(tcpSocket), isInitialized(false), infoHash(infoHash), mFile(mFile), amchoked(true), aminterested(false), peerchoked(true), peerinterested(false)
{
	connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(this, SIGNAL(pieceDownloaded(size_t)), parent, SLOT(onPieceDownloaded(size_t)));

}

PeerConnection::~PeerConnection()
{
}

void PeerConnection::choke()
{
	std::string message = "00010";
	socket->write(message.data(), message.size());
}

void PeerConnection::unchoke()
{
	std::string message = "00011";
	socket->write(message.data(), message.size());
}

void PeerConnection::interested()
{
	std::string message = "00012";
	socket->write(message.data(), message.size());
}

void PeerConnection::not_interested()
{
	std::string message = "00013";
	socket->write(message.data(), message.size());
}

void PeerConnection::have(int index)
{
	std::string idx = std::to_string(index);
	std::reverse(idx.begin(), idx.end());
	idx.resize(4, '0');
	std::reverse(idx.begin(), idx.end());
	std::string message = "00054" + idx;
	socket->write(message.data(), message.size());
}

void PeerConnection::bitfield(BitSet& pieces)
{

	size_t size = pieces.getSize() + 1;
	std::string message = std::to_string(size) + "5" + pieces.to_string();
	socket->write(message.data(), message.size());
}

void PeerConnection::request(int index, int begin, int length)
{	
	std::string idx = std::to_string(index);
	std::reverse(idx.begin(), idx.end());
	idx.resize(4, '0');
	std::reverse(idx.begin(), idx.end());

	std::string beg = std::to_string(begin);
	std::reverse(beg.begin(), beg.end());
	beg.resize(4, '0');
	std::reverse(beg.begin(), beg.end());

	std::string len = std::to_string(length);
	std::reverse(len.begin(), len.end());
	len.resize(4, '0');
	std::reverse(len.begin(), len.end());

	std::string message = "00136" + idx + beg + len;
	socket->write(message.data(), message.size());
}

void PeerConnection::piece(int index, int begin, std::string block)
{	
	int blocksize = 9 + block.size();

	std::string size = std::to_string(blocksize);
	std::reverse(size.begin(), size.end());
	size.resize(4, '0');
	std::reverse(size.begin(), size.end());

	std::string idx = std::to_string(index);
	std::reverse(idx.begin(), idx.end());
	idx.resize(4, '0');
	std::reverse(idx.begin(), idx.end());

	std::string beg = std::to_string(begin);
	std::reverse(beg.begin(), beg.end());
	beg.resize(4, '0');
	std::reverse(beg.begin(), beg.end());

	std::string message = size + "7" + idx + beg + block;
	socket->write(message.data(), message.size());
}

void PeerConnection::handshake()
{
	std::string message = infoHash + Client::getInstance()->getId();
	socket->write(message.data(), message.size());
}

void PeerConnection::readData()
{
	buffor += socket->readAll().toStdString();

	if (buffor.size() >= 4)
	{
		int message_size = std::stoi(buffor.substr(0, 4));
		if (message_size <= buffor.size() - 4)
		{
			std::string message = buffor.substr(5, message_size);
			if (message[0] == '0')
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
			}
			else if (message[0] == '4')
			{

			}
			else if (message[0] == '5')
			{

			}
			else if (message[0] == '6')
			{

			}
			else if (message[0] == '7')
			{

			}
			buffor.erase(0, message_size + 4);
		}
	}
	//mFile->setFrag(frag, index);
	//emit pieceDownloaded(index);
}
