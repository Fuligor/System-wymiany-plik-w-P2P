#include "Torrent.h"

#include <QFile>

#include "Bencode.h"
#include "Client.h"
#include "InfoDictHash.h"
#include "TorrentDownloader.h"
#include "TorrentManager.h"
#include "torrentReader.h"


#include <iostream>

void Torrent::initialize(const std::string& fileHash)
{
	downloader = new TorrentDownloader(torrentDict, status.pieceStatus, this);
	TorrentManager::getInstance().append(fileHash, this);
	connect(downloader, SIGNAL(statusUpdated()), this, SLOT(downloadStatusUpdated()));
	connect(this, SIGNAL(torrentStatusUpdated(const std::string, const TorrentDownloadStatus*)), &TorrentManager::getInstance(), SIGNAL(torrentStatusUpdated(const std::string, const TorrentDownloadStatus*)));

	downloadStatusUpdated();
}

Torrent::Torrent(const std::string& fileHash)
	:status(0)
{
	std::string filePath = Client::getConfigPath() + "/" + fileHash;
	file = new QFile(filePath.c_str());
	file->open(QIODevice::ReadWrite);

	read();
	torrentReader reader(status.torrentPath);
	torrentDict = reader.getDict();

	initialize(fileHash);
}

Torrent::Torrent(const std::string& torrendPath, const std::string& downloadPath)
	:status(0)
{
	status.torrentPath = torrendPath;

	torrentReader reader(status.torrentPath);
	torrentDict = reader.getDict();

	std::shared_ptr <bencode::Dict> infoDict = std::dynamic_pointer_cast <bencode::Dict> ((*torrentDict)["info"]);
	std::wstring* torrentName = dynamic_cast <bencode::String*> ((*infoDict)["name"].get());

	status.downloadPath = downloadPath;
	size_t pieceCount = std::dynamic_pointer_cast <bencode::String> ((*infoDict)["pieces"])->size() / 20;

	status.pieceStatus = BitSet(pieceCount);
	status.pieceStatus.set();
	std::string fileHash = InfoDictHash::getReadableHash(torrentDict);

	std::string filePath = Client::getConfigPath() + "/" + fileHash;

	std::cout << filePath << std::endl;
	file = new QFile(filePath.c_str());
	if(!file->open(QIODevice::ReadWrite))
	{
		std::cout << file->errorString().toStdString();
	}

	write();

	initialize(fileHash);
}

Torrent::~Torrent()
{
	delete file;
}

void Torrent::write()
{
	const size_t pieceCount = status.pieceStatus.getSize();
	QByteArray buf;

	file->seek(0);

	file->write((char*) &pieceCount, sizeof(pieceCount));
	file->write(status.pieceStatus.getData(), status.pieceStatus.getDataSize());

	buf = QString::fromStdString(status.torrentPath).toUtf8();

	const size_t torrentPathLenght = buf.size();
	write(torrentPathLenght);
	file->write(buf);

	buf = QString::fromStdString(status.downloadPath).toUtf8();

	const size_t downloadPathLenght = buf.size();
	write(downloadPathLenght);
	file->write(buf);

	file->flush();
}

void Torrent::write(size_t size)
{
	for(size_t i = 0; i < sizeof(size); ++i)
	{
		unsigned char temp = size % 256;

		file->write((char *) &temp, 1);

		size = size >> 8;
	}
}

void Torrent::read()
{
	size_t pieceCount;
	size_t torrentPathLenght;
	size_t downloadPathLenght;
	size_t connectionIdSize;
	char* buffer;
	QByteArray buf;

	file->seek(0);

	read(pieceCount);
	buffer = new char[pieceCount];

	file->read(buffer, BitSet::getPageNumber(pieceCount));
	status.pieceStatus = BitSet((unsigned char*) buffer, pieceCount);

	read(torrentPathLenght);

	buf = file->read(torrentPathLenght);
	status.torrentPath = QString::fromUtf8(buf).toStdString();

	read(downloadPathLenght);

	buf = file->read(downloadPathLenght);
	status.downloadPath = QString::fromUtf8(buf).toStdString();
}

void Torrent::read(size_t& size)
{
	size = 0;

	for (size_t i = 0; i < sizeof(size); ++i)
	{
		unsigned char temp;

		file->read((char *) &temp, 1);

		size = size + ((size_t) temp <<  (i * 8));
	}
}

void Torrent::updatePage(const size_t page)
{
	file->seek(sizeof(size_t) + page);

	file->putChar(status.pieceStatus.getData()[page]);
}

const TorrentConfig* Torrent::getStatus() const
{
	return &status;
}

void Torrent::onPieceDownloaded(const size_t& index)
{
	status.pieceStatus.set(index);

	updatePage(BitSet::getPageNumber(index));
}

void Torrent::downloadStatusUpdated()
{
	//std::cout << file->fileName().toStdString() << std::endl;

	emit torrentStatusUpdated(file->fileName().toStdString(), &(downloader->getDownloadStatus()));
}