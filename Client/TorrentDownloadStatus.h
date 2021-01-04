#ifndef TORRENT_DOWNLOAD_STATUS
#define TORRENT_DOWNLOAD_STATUS
#include <stddef.h>

#include <QDateTime>
#include <QTime>

#include "FileSize.h"

struct TorrentDownloadStatus
{
	FileSize downloaded;
	FileSize uploaded;
	FileSize downloadedSinceStart;
	FileSize downloadSpeed;
	FileSize uploadSpeed;
	QTime estimatedEndTime;
	QDateTime startTime;
	std::wstring fileName;
	FileSize fileSize;
	size_t connectionCount;
	enum class State
	{
		CONNECTING_TO_TRACKER,
		TRACKER_CONNECTION_REFUSED,
		LEECHING,
		SEEDING,
		CLOSED
	} connectionState;
};
#endif // !TORRENT_DOWNLOAD_STATUS
