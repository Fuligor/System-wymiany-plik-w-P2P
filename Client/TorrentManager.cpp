#include "TorrentManager.h"

#include <stdexcept>

#include <QDir>

#include "Client.h"
#include "TorrentFileStatus.h"
#include "torrentReader.h"

TorrentManager::TorrentManager()
{
}

void TorrentManager::updateDownloadList()
{
	QDir directory(Client::getConfigPath().c_str());

	for (auto i : directory.entryList(QDir::Filter::Files))
	{
		std::string temp = i.toStdString();

		fileList[temp] = new Torrent(temp);
	}
}

TorrentManager::~TorrentManager()
{
	for(auto i: fileList)
	{
		if(i.second == nullptr)
		{
			continue;
		}

		delete i.second;
	}
}

TorrentManager& TorrentManager::getInstance()
{
	if(manager == nullptr)
	{
		manager = new TorrentManager;
	}

	return *manager;
}

Torrent* const TorrentManager::operator[](const std::string& infoHash)
{
	std::string temp = infoHash;

	if(fileList[temp] == nullptr)
	{
		throw std::domain_error("Couldn't find file!");
	}

	return fileList[temp];
}

void TorrentManager::append(const std::string& infoHash, Torrent* torrent)
{
	fileList[infoHash] = torrent;
}

TorrentManager* TorrentManager::manager = nullptr;