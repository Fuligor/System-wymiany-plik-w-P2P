#include "TorrentManager.h"

#include <stdexcept>

#include <QDir>

#include "Client.h"
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

		if (temp.find('.') == std::string::npos)
		{
			Torrent* torrent = nullptr;

			try
			{ 
				torrent = new Torrent(temp);

				fileList[temp] = torrent;
			}
			catch (std::domain_error& e)
			{
				emit wrongConfigFile(e.what());
			}
			catch (std::exception& e)
			{
				removeFile(temp);

				emit wrongConfigFile("Wyst¹pi³ problem z plikiem konfikuracyjnym  " + temp + " i zostanie on usuniêty. \n Kod b³êdu: " + e.what());
			}
		}
	}
}

TorrentManager::~TorrentManager()
{
	clear();
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
		emit wrongConfigFile("Nie mo¿na znaleŸæ szukanego pliku!");
	}

	return fileList[temp];
}

void TorrentManager::append(const std::string& infoHash, Torrent* torrent)
{
	fileList[infoHash] = torrent;
}

void TorrentManager::remove(const std::string& infoHash)
{
	if(fileList[infoHash] != nullptr)
	{
		fileList[infoHash]->deleteFile();
		fileList[infoHash]->deleteLater();
		fileList.erase(infoHash);
	}
}

void TorrentManager::removeFile(const std::string& infoHash)
{
	std::string filePath = Client::getConfigPath() + "/" + infoHash;
	QFile file(filePath.c_str());
	file.remove(filePath.c_str());

	return;
}

void TorrentManager::clear()
{
	for(auto i: fileList)
	{
		i.second->deleteLater();
	}

	fileList.clear();
}

TorrentManager* TorrentManager::manager = nullptr;