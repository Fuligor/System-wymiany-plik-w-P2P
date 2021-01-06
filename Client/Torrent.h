#ifndef TORRENT_MANAGER_FILE_H
#define TORRENT_MANAGER_FILE_H
#include <QObject>

#include <memory>

#include <QMutex>

#include "Bencode.h"
#include "TorrentConfig.h"

class QFile;

struct TorrentDownloadStatus;

class TorrentDownloader;

class Torrent
	:public QObject
{
	Q_OBJECT
private:
	QMutex mutex;
	TorrentConfig status;
	std::shared_ptr <bencode::Dict> torrentDict;
	TorrentDownloader* downloader;
	QFile* file;

	void initialize(const std::string& fileHash);
public:
	Torrent(const std::string& fileHash);
	Torrent(const std::string& torrendPath, const std::string& downloadPath, bool upload = false);
	~Torrent();

	void deleteFile();
protected:
	void write();
	void write(uint64_t size);
	void read();
	void read(uint64_t& size);
	void updatePage(const uint64_t page);
public:
	const TorrentConfig* getStatus() const;
public slots:
	void onPieceDownloaded(const uint64_t& index);
	void onPieceUploaded(const uint64_t& pieceSize);
	void downloadStatusUpdated();
	void downloadSpeedUpdated();
	void newDownloadConnection();
signals:
	void torrentStatusUpdated(const std::string, const TorrentDownloadStatus*);
	void torrentPieceDownloaded(const TorrentDownloadStatus*);
	void torrentPieceUploaded(const TorrentDownloadStatus*);
	void torrentSpeedUpdated(const TorrentDownloadStatus*);
	void torrentNewConnection(const TorrentDownloadStatus*);
};

#endif