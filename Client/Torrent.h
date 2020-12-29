#ifndef TORRENT_MANAGER_FILE_H
#define TORRENT_MANAGER_FILE_H
#include <QObject>

#include <memory>

#include "Bencode.h"
#include "TorrentFileStatus.h"

class QFile;

struct TorrentDownloadStatus;

class TorrentDownloader;

class Torrent
	:public QObject
{
	Q_OBJECT
private:
	TorrentFileStatus status;
	std::shared_ptr <bencode::Dict> torrentDict;
	TorrentDownloader* downloader;
	QFile* file;

	void initialize(const std::string& fileHash);
public:
	Torrent(const std::string& fileHash);
	Torrent(const std::wstring& torrendPath, const std::wstring& downloadPath);
	~Torrent();
protected:
	void write();
	void write(size_t size);
	void read();
	void read(size_t& size);
public:
	const TorrentFileStatus* getStatus() const;
public slots:
	void pieceDownloaded(const size_t& index);
	void downloadStatusUpdated();
signals:
	void torrentStatusUpdated(const std::string, const TorrentDownloadStatus*);
};

#endif