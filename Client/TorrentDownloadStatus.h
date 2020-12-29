#ifndef TORRENT_DOWNLOAD_STATUS
#define TORRENT_DOWNLOAD_STATUS

#include <QDateTime>
#include <QTime>

struct TorrentDownloadStatus
{
	double downloaded;
	double uploaded;
	double downloadSpeed;
	double uploadSpeed;
	QTime estimatedEndTime;
	QDateTime startTime;
	std::wstring fileName;
	size_t fileSize;
};
#endif // !TORRENT_DOWNLOAD_STATUS
