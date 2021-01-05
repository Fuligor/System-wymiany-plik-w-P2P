#ifndef TORRENT_MANAGER_H
#define TORRENT_MANAGER_H
#include <QObject>

#include <map>
#include <string>

#include "Torrent.h"

class TorrentManager:
	public QObject
{
	Q_OBJECT
private:
	static TorrentManager* manager;

	std::map <std::string, Torrent*> fileList;

	TorrentManager();
public:
	~TorrentManager();

	static TorrentManager& getInstance();
	void updateDownloadList();

	Torrent* const operator[](const std::string& infoHash);
	void append(const std::string& infoHash, Torrent* torrent);
	void remove(const std::string& infoHash);
protected:
	void removeFile(const std::string& infoHash);
signals:
	void torrentStatusUpdated(const std::string, const TorrentDownloadStatus*);
	void wrongConfigFile(std::string);
public slots:
	void clear();
};

#endif